// Copyright 2024 XD Games, Inc. All Rights Reserved.

/*=============================================================================
    UnrealMultiLog.h

    Author: Zhang, HaiJun

    Desc:
=============================================================================*/


#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "MultiThreadedLogDevice.h"
#include "HoloLens/HoloLensPlatformOutputDevices.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/OutputDeviceFile.h"


#define container_of(ptr, type, member) \
    reinterpret_cast<type*>(reinterpret_cast<char*>(ptr) - offsetof(type, member))




class FUnrealMultiLogModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:

    static  FString GetUnrealLogFilePath()
    {
        //  return FPaths::ProjectLogDir() / (UKismetSystemLibrary::GetGameName() + TEXT(".log"));
        FString GameLog = "";
        if (MultiThreadedLogDevice)
        {
            GameLog = MultiThreadedLogDevice->GetLogFilePath();
        }

        return GameLog;

    }

    void InitializeMultiThreadedLogging()
    {
        // Add the custom log device to the global log system
        if (GLog)
        {
            FString GameLog = "";
            OldLog = FPlatformOutputDevices::GetLog();
            FOutputDeviceFile* OldLogDeviceFile = static_cast<FOutputDeviceFile*>(OldLog);
            if (OldLogDeviceFile)
            {
                GameLog = OldLogDeviceFile->GetFilename();
            }
            GLog->EnableBacklog(true);
            GLog->RemoveOutputDevice(OldLog);
            OldLog->Flush();
/*            OldLog->TearDown();*/

          
            MultiThreadedLogDevice = new FMultiThreadedLogDevice(GameLog);
            GLog->AddOutputDevice(MultiThreadedLogDevice);
            GLog->EnableBacklog(true);
          
            FString LogFilePath = GetUnrealLogFilePath();
            FOutputDeviceFile* NewLogDeviceFile = static_cast<FOutputDeviceFile*>(OldLog);
            if (NewLogDeviceFile)
            {
                NewLogDeviceFile->SetFilename(*GameLog);
            }
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

       FMultiThreadedLogDevice* GetMultiThreadedLogDevice()
       {
           return MultiThreadedLogDevice;
       }

private:
    static  FMultiThreadedLogDevice* MultiThreadedLogDevice;

    static FOutputDevice* OldLog;
};
