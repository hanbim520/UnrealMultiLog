// Copyright ZhangHaiJun 710605420@qq.com, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "MultiThreadedLogDevice.h"
#include "HoloLens/HoloLensPlatformOutputDevices.h"
#include "Kismet/KismetSystemLibrary.h"


static  FString GetLogFilePath()
{
    return FPaths::ProjectLogDir() / (UKismetSystemLibrary::GetGameName() + TEXT(".log"));
}

class FUnrealMultiLogModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
    void InitializeMultiThreadedLogging()
    {
        // Add the custom log device to the global log system
        if (GLog)
        {
            FOutputDevice* OldLog = FPlatformOutputDevices::GetLog();
            GLog->RemoveOutputDevice(OldLog);
            OldLog->Flush();
            OldLog->TearDown();

            FString LogFilePath = GetLogFilePath();
            MultiThreadedLogDevice = new FMultiThreadedLogDevice(LogFilePath);

            GLog->AddOutputDevice(MultiThreadedLogDevice);
            GLog->EnableBacklog(true);
        }
    }

    void ShutdownMultiThreadedLogging()
    {
        if (MultiThreadedLogDevice)
        {
            // Remove the custom log device from the global log system
            if (GLog)
            {
                GLog->RemoveOutputDevice(MultiThreadedLogDevice);
            }

            delete MultiThreadedLogDevice;
            MultiThreadedLogDevice = nullptr;
        }
    }

    
public:
       static void SignalHandler(int signal);

       void HandleShutdownAfterError();

private:
    static  FMultiThreadedLogDevice* MultiThreadedLogDevice;
};
