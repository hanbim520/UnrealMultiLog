// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MultiThreadedLogDevice.h"
#include "Android/AndroidPlatformOutputDevices.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MUTIPLELOGTEST_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
//     void InitializeMultiThreadedLogging()
//     {
//         FString LogFilePath = FPaths::ProjectLogDir() / TEXT("MultiThreadedLog.txt");
//         MultiThreadedLogDevice = new FMultiThreadedLogDevice(LogFilePath);
//        
//         // Add the custom log device to the global log system
//         if (GLog)
//         {
// 
//             GLog->RemoveOutputDevice(FPlatformOutputDevices::GetLog());
//            
//             GLog->AddOutputDevice(MultiThreadedLogDevice);
//         }
//     }
// 
//     void ShutdownMultiThreadedLogging()
//     {
//         if (MultiThreadedLogDevice)
//         {
//             // Remove the custom log device from the global log system
//             if (GLog)
//             {
//                 GLog->RemoveOutputDevice(MultiThreadedLogDevice);
//             }
// 
//             delete MultiThreadedLogDevice;
//             MultiThreadedLogDevice = nullptr;
//         }
//     }

public:
    virtual void Init() override;
    virtual void Shutdown() override;

private:
   // FMultiThreadedLogDevice* MultiThreadedLogDevice = nullptr;
};
