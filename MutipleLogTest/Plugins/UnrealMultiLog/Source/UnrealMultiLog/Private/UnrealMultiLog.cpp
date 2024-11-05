// Copyright ZhangHaiJun 710605420@qq.com, Inc. All Rights Reserved.

#include "UnrealMultiLog.h"

#define LOCTEXT_NAMESPACE "FUnrealMultiLogModule"
FMultiThreadedLogDevice* FUnrealMultiLogModule::MultiThreadedLogDevice = nullptr;


const int32 IgnoreCount = 1; // Ignore the current function in the stack trace
const SIZE_T StackBufferSize = 4096; // Size of the buffer to hold the stack trace
ANSICHAR StackTraceBuffer[StackBufferSize] = {0};

static const int MAX_DEPTH = 200;
uint64 StackTrace[MAX_DEPTH] = {0};

void StackWalkAndDump(ANSICHAR* InHumanReadableString, SIZE_T InHumanReadableStringSize, int32 InIgnoreCount = 1, void* InContext = nullptr)
{
	// If the callstack is for the executing thread, ignore this function and the FPlatformStackWalk::CaptureStackBackTrace call below
	if (InContext == nullptr)
	{
		InIgnoreCount += 2;
	}

	// Capture stack backtrace.
	uint32 Depth = FPlatformStackWalk::CaptureStackBackTrace(StackTrace, MAX_DEPTH, InContext);

	// Skip the first two entries as they are inside the stack walking code.
	uint32 CurrentDepth = InIgnoreCount;
	while (CurrentDepth < Depth)
	{
		FPlatformStackWalk::ProgramCounterToHumanReadableString(CurrentDepth, StackTrace[CurrentDepth], InHumanReadableString, InHumanReadableStringSize, reinterpret_cast<FGenericCrashContext*>(InContext));
		FCStringAnsi::Strncat(InHumanReadableString, LINE_TERMINATOR_ANSI, (int32)InHumanReadableStringSize);
		CurrentDepth++;
	}
}

void CaptureStackTrace(ANSICHAR* OutStackTrace, SIZE_T InBufferSize)
{
	// Call the StackWalkAndDump function
	//FPlatformStackWalk::StackWalkAndDump(StackTraceBuffer, InBufferSize, IgnoreCount);
	StackWalkAndDump(StackTraceBuffer, InBufferSize, IgnoreCount);
}


void FUnrealMultiLogModule::HandleShutdownAfterError()
{
    CaptureStackTrace(StackTraceBuffer, StackBufferSize);
    if (MultiThreadedLogDevice)
    {
        MultiThreadedLogDevice->CrashCaptureLog(StackTraceBuffer);
    }
	if (GError)
	{
		GError->Flush();
		GError->HandleError();
	}
	
	if (GLog)
	{
		GLog->Flush();
	}

}

void FUnrealMultiLogModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	InitializeMultiThreadedLogging();
	//FCoreDelegates::OnHandleSystemError.AddRaw(this, &FUnrealMultiLogModule::HandleShutdownAfterError);
    //FCoreDelegates::OnShutdownAfterError.AddRaw(this, &FUnrealMultiLogModule::HandleShutdownAfterError);

}

void FUnrealMultiLogModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	ShutdownMultiThreadedLogging();
   // FCoreDelegates::OnHandleSystemError.RemoveAll(this);
	//FCoreDelegates::OnShutdownAfterError.RemoveAll(this);


}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealMultiLogModule, UnrealMultiLog)