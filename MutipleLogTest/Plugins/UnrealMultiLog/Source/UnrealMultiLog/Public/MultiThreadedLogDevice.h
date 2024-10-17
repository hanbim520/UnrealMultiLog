// Copyright ZhangHaiJun 710605420@qq.com, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Misc/FileHelper.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "Misc/OutputDevice.h"
#include "Misc/Paths.h"
#include "Async/Async.h"
#include "TDynamicConcurrentQueue.h"

/**
 * Multi-threaded log device for writing UE_LOG messages to a file without blocking the main thread.
 */
class UNREALMULTILOG_API FMultiThreadedLogDevice : public FOutputDevice, public FRunnable
{
public:
    // Constructor
    FMultiThreadedLogDevice(const FString& InLogFilePath)
        : LogFilePath(InLogFilePath), bIsRunning(true)
    {
        // Create the thread for writing logs
        Thread = FRunnableThread::Create(this, TEXT("FMultiThreadedLogWriter"), 0, TPri_BelowNormal);
    }

    // Destructor
    virtual ~FMultiThreadedLogDevice()
    {
        bIsRunning = false;

        // Ensure all logs are written before shutting down
        if (Thread)
        {
            Thread->WaitForCompletion();
            delete Thread;
            Thread = nullptr;
        }
    }

    // Override the Serialize function to capture UE_LOG messages
    virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category) override
    {
        // Format the log message and enqueue it for the background thread
        TUniquePtr<FString> LogEntry = MakeUnique<FString>(FString::Printf(TEXT("[%s] [%s]: %s"), *Category.ToString(), GetVerbosityName(Verbosity), V));
        LogQueue.Enqueue(MoveTemp(LogEntry));  // No copy of the string, move to queue
    }

    // Main run function for the background thread
    virtual uint32 Run() override
    {
        while (bIsRunning || !LogQueue.IsEmpty())
        {
            WriteLogsToFile();
            FPlatformProcess::Sleep(0.01f);  // Control the frequency of log writing
        }
        return 0;
    }

    virtual void Exit() override
    {
        WriteLogsToFile();  // Write any remaining logs before exit
    }

private:
    FString LogFilePath;  // The path to the log file
    TDynamicConcurrentQueue<TUniquePtr<FString>> LogQueue;  // Lock-free queue for log messages
    FRunnableThread* Thread;  // The background thread for writing logs
    bool bIsRunning;  // Flag to control the running state of the thread

    // Helper to write logs from the queue to the file
    void WriteLogsToFile()
    {
        TUniquePtr<FString> LogEntry;
        while (LogQueue.Dequeue(LogEntry))  // Dequeue the log message from the queue
        {
            if (LogEntry.IsValid())
            {
                // Append the log message to the file
                FFileHelper::SaveStringToFile(*LogEntry + LINE_TERMINATOR, *LogFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
            }
        }
    }

    // Helper to get the string representation of log verbosity
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
