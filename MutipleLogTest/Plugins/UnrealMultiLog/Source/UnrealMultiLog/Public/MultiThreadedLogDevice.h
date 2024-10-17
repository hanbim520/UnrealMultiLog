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
        Stop();
    }

    void Stop()
    {
        bIsRunning.store(false); // Set atomic flag to false

        // Wait for the thread to complete
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

    virtual uint32 Run() override
    {
        while (bIsRunning.load() || !LogQueue.IsEmpty())
        {
            WriteLogsToFile();
            FPlatformProcess::Sleep(0.01f);  // Control the frequency of log writing
        }
        return 0;
    }

    // This function will be called on pre-exit
    void OnPreExit()
    {
        Stop(); // Safely stop the thread
        WriteLogsToFile();  // Write any remaining logs
    }

private:
    FString LogFilePath;
    TDynamicConcurrentQueue<TUniquePtr<FString>> LogQueue;
    FRunnableThread* Thread;
    std::atomic<bool> bIsRunning; // Use atomic flag for thread-safe access without locks

    void WriteLogsToFile()
    {
        QUICK_SCOPE_CYCLE_COUNTER(FMultiThreadedLogDevice_WriteLogsToFile)
            TUniquePtr<FString> LogEntry;
        while (LogQueue.Dequeue(LogEntry))
        {
            if (LogEntry.IsValid())
            {
                FFileHelper::SaveStringToFile(*LogEntry + LINE_TERMINATOR, *LogFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
            }
        }
    }

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
