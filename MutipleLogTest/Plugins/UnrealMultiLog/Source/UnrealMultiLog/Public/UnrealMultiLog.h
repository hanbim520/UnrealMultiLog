// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "MultiThreadedLogDevice.h"
#include "HoloLens/HoloLensPlatformOutputDevices.h"
#include "Kismet/KismetSystemLibrary.h"

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

            FString LogFilePath = FPaths::ProjectLogDir() / UKismetSystemLibrary::GetGameName() + TEXT(".log");
            MultiThreadedLogDevice = new FMultiThreadedLogDevice(LogFilePath);

            GLog->AddOutputDevice(MultiThreadedLogDevice);
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

private:
    FMultiThreadedLogDevice* MultiThreadedLogDevice = nullptr;
};
