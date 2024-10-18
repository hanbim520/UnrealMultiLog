// Copyright ZhangHaiJun 710605420@qq.com, Inc. All Rights Reserved.

#include "UnrealMultiLog.h"

#define LOCTEXT_NAMESPACE "FUnrealMultiLogModule"
FMultiThreadedLogDevice* FUnrealMultiLogModule::MultiThreadedLogDevice = nullptr;


void CaptureStackTrace(FString& OutStackTrace)
{
	const int32 IgnoreCount = 4; // Ignore the current function in the stack trace
	const SIZE_T BufferSize = 2048; // Size of the buffer to hold the stack trace
	ANSICHAR StackTraceBuffer[BufferSize];

	// Call the StackWalkAndDump function
	FPlatformStackWalk::StackWalkAndDump(StackTraceBuffer, BufferSize, IgnoreCount);

	// Convert the buffer to an FString
	OutStackTrace = FString(UTF8_TO_TCHAR(StackTraceBuffer));
}


void FUnrealMultiLogModule::HandleShutdownAfterError()
{
    FString StackTrace;
    CaptureStackTrace(StackTrace);
    if (MultiThreadedLogDevice)
    {
        MultiThreadedLogDevice->CrashCaptureLog(StackTrace);
    }

}

void FUnrealMultiLogModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	InitializeMultiThreadedLogging();

    FCoreDelegates::OnShutdownAfterError.AddRaw(this, &FUnrealMultiLogModule::HandleShutdownAfterError);
}

void FUnrealMultiLogModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	ShutdownMultiThreadedLogging();
    FCoreDelegates::OnShutdownAfterError.RemoveAll(this);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealMultiLogModule, UnrealMultiLog)