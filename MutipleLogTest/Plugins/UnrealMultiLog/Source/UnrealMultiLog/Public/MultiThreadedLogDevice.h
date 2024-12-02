// Copyright 2024 XD Games, Inc. All Rights Reserved.

/*=============================================================================
    MultiThreadedLogDevice.h

    Author: Zhang, HaiJun

    Desc:
=============================================================================*/


#pragma once

#include "CoreMinimal.h"
#include "Misc/FileHelper.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "Misc/OutputDevice.h"
#include "Misc/Paths.h"
#include "Async/Async.h"
#include "TDynamicConcurrentQueue.h"
#include "HAL/PlatformProcess.h"
#include "Misc/OutputDeviceRedirector.h"
#include <atomic>
#include "HAL/ExceptionHandling.h"
#include <fcntl.h>
#include <string>
#include "Kismet/KismetSystemLibrary.h"


#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "corecrt_io.h"
#include <winnt.h>  

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#else
#include <unistd.h>
#endif
#include "Misc/OutputDeviceHelper.h"
#include "CoreGlobals.h"

#define  PERMISSIONS 0777


/**
 * Multi-threaded log device for writing UE_LOG messages and crash stack traces to a file without blocking the main thread.
 */
class UNREALMULTILOG_API FMultiThreadedLogDevice : public FOutputDevice, public FRunnable
{
public:
    // Constructor
    FMultiThreadedLogDevice(const FString& InLogFilePath)
        : LogFilePath(*InLogFilePath)
        , bIsRunning(true)
        , LogFilePathStr(TCHAR_TO_UTF8(*InLogFilePath))
    {
        // Create the thread for writing logs
        Thread = FRunnableThread::Create(this, TEXT("FMultiThreadedLogWriter"), 0, TPri_BelowNormal);

        TUniquePtr<FString> LogEntry = MakeUnique<FString>(TEXT("Log file created at: ") + FDateTime::Now().ToString());
        //FFileHelper::SaveStringToFile(*LogEntry + LINE_TERMINATOR, *LogFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
    }

    // Destructor
    virtual ~FMultiThreadedLogDevice()
    {
        Stop();
    }

    // Stop the logging thread
    void Stop()
    {
        bIsRunning.store(false); // Set atomic flag to false

        // Wait for the thread to complete
        if (Thread)
        {
            Thread->WaitForCompletion();
            Thread = nullptr;
        }
    }

    // Override the Serialize function to capture UE_LOG messages
    virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category, const double Time) override
    {
        const double RealTime = Time == -1.0f ? FPlatformTime::Seconds() - GStartTime : Time;
        TCHAR OutputString[MAX_SPRINTF] = TEXT(""); //@warning: this is safe as FCString::Sprintf only use 1024 characters max
        FCString::Sprintf(OutputString, TEXT("%s%s"), *FOutputDeviceHelper::FormatLogLine(Verbosity, Category, V, GPrintLogTimes, RealTime), TEXT("\n"));
        std::string LogEntry = TCHAR_TO_UTF8(OutputString);
        LogQueue.Enqueue(std::move(LogEntry));
    }
    virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category) override
    {

    }
    virtual void SerializeForOnline(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category,FString& time) 
    {
        // Format the log message and enqueue it for the background thread
//         TUniquePtr<FString> LogEntry = MakeUnique<FString>(FString::Printf(TEXT("[%s][%s][%s]: %s\n"),*time, *Category.ToString(), GetVerbosityName(Verbosity), V));
// 
//         FTCHARToUTF8 Converted(**LogEntry.Get());
//         LogQueue.Enqueue(MakeUnique<std::string>(std::move(Converted.Get())));  // Move to queue without copying
        
        std::string TimeStdString = TCHAR_TO_UTF8(*time);
        std::string CategoryStdString = TCHAR_TO_UTF8(*Category.ToString());
        std::string VerbosityStdString = TCHAR_TO_UTF8(GetVerbosityName(Verbosity));
        std::string LogEntry = std::string("[")
            + TimeStdString
            + std::string("]")
            + std::string("[")
            + std::string(TCHAR_TO_UTF8(*Category.ToString()))
            + std::string("]")
            + std::string("[")
            + std::string(TCHAR_TO_UTF8(*GetVerbosityName(Verbosity)))
            + std::string("]")
            + std::string(TCHAR_TO_UTF8(V))
            + std::string("\n");

        LogQueue.Enqueue(std::move(LogEntry));
    }

    void CaptureStackTrace(FString& OutStackTrace)
    {
        const int32 IgnoreCount = 0; // Ignore the current function in the stack trace
        const SIZE_T BufferSize = 2048; // Size of the buffer to hold the stack trace
        ANSICHAR StackTraceBuffer[BufferSize];

        // Call the StackWalkAndDump function
        FPlatformStackWalk::StackWalkAndDump(StackTraceBuffer, BufferSize, IgnoreCount);

        // Convert the buffer to an FString
        OutStackTrace = FString(UTF8_TO_TCHAR(StackTraceBuffer));
    }

    // The main run function for the logging thread
    virtual uint32 Run() override
    {
        while (bIsRunning.load() || !LogQueue.IsEmpty())
        {
          //  WriteLogsToFile();  // Write logs to the file
            LowLevelWriteLogsToFile();
            FPlatformProcess::Sleep(0.01f);  // Control the frequency of log writing
        }
        return 0;
    }

    void CrashCaptureLog(char* StackTraceBuffer)
    {
      //  int fd = open(LogFilePathStr.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0644);
        int fd = open(LogFilePathStr.c_str(), O_WRONLY | O_APPEND | O_CREAT , PERMISSIONS);
        if (fd != -1)
        {
         //   FTCHARToUTF8 Converted(*StackTrace);
          //  write(fd, Converted.Get(), Converted.Length());
            size_t BytesWritten = write(fd, StackTraceBuffer, strlen(StackTraceBuffer));
#if PLATFORM_WINDOWS
#ifdef _WIN32
            // Windows-specific: Flush file buffers to ensure data is written to disk
            HANDLE hFile = (HANDLE)_get_osfhandle(fd); // Convert the file descriptor to a Windows handle
            if (hFile != INVALID_HANDLE_VALUE)
            {
                FlushFileBuffers(hFile);  // Force the buffer to flush to disk
            }
#endif
#endif
            close(fd);
        }
    }
    // Forcefully flush remaining logs to the file
    void ForceFlushLogs()
    {
        while (!LogQueue.IsEmpty())
        {
           // WriteLogsToFile();  // Write remaining logs

            LowLevelWriteLogsToFile();
        }
    }

    // This function will be called on pre-exit
    void OnPreExit()
    {
        Stop(); // Safely stop the thread
        ForceFlushLogs();  // Write any remaining logs
    }

    FString GetLogFilePath();
private:
    FString LogFilePath;  // Path to the log file
    TDynamicConcurrentQueue<std::string> LogQueue;  // Thread-safe queue for log entries
    FRunnableThread* Thread;  // Thread for writing logs
    std::atomic<bool> bIsRunning; // Atomic flag for thread-safe access without locks
    std::string LogFilePathStr;

private:
    // Write logs from the queue to the file
//     void WriteLogsToFile()
//     {
//         QUICK_SCOPE_CYCLE_COUNTER(FMultiThreadedLogDevice_WriteLogsToFile)
//             TUniquePtr<std::string> LogEntry;
//         while (LogQueue.Dequeue(LogEntry))
//         {
//             if (LogEntry.IsValid())
//             {
//                 FFileHelper::SaveStringToFile(*LogEntry + LINE_TERMINATOR, *LogFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
//             }
//         }
//     }

    void LowLevelWriteLogsToFile()
    {
        QUICK_SCOPE_CYCLE_COUNTER(FMultiThreadedLogDevice_LowLevelWriteLogsToFile)

        // Open the log file once and keep it open for multiple writes
        int fd = open(LogFilePathStr.c_str(), O_WRONLY | O_APPEND | O_CREAT, PERMISSIONS);
        if (fd == -1)
        {
            // If the file cannot be opened, exit the function early
            return;
        }

        std::string LogEntry;
        while (LogQueue.Dequeue(LogEntry))
        {
            if (!LogEntry.empty())
            {
                // Write log entry to file
                write(fd, LogEntry.c_str(), LogEntry.length());
            }
        }

        // Close the file after all logs have been written
        close(fd);
    }

    // Get the string representation of the log verbosity
    const TCHAR* GetVerbosityName(ELogVerbosity::Type Verbosity)
    {
        switch (Verbosity)
        {
        case ELogVerbosity::Fatal: return TEXT("Fatal");
        case ELogVerbosity::Error: return TEXT("Error");
        case ELogVerbosity::Warning: return TEXT("Warning");
        case ELogVerbosity::Display: return TEXT("Display");
        case ELogVerbosity::Log: return TEXT("Log");
        case ELogVerbosity::Verbose: return TEXT("Verbose");
        case ELogVerbosity::VeryVerbose: return TEXT("VeryVerbose");
        default: return TEXT("Unknown");
        }
    }
};
