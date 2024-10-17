// Copyright ZhangHaiJun 710605420@qq.com, Inc. All Rights Reserved.

#include "UnrealMultiLog.h"
// #include <csignal>
// #include <chrono>
// #include <thread>

#define LOCTEXT_NAMESPACE "FUnrealMultiLogModule"


// void SignalHandler(int signal) {
// 	// TO-DO something
// }


void FUnrealMultiLogModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

// 	std::signal(SIGINT, SignalHandler);
// 	std::signal(SIGILL, SignalHandler);
// 	std::signal(SIGFPE, SignalHandler);
// 	std::signal(SIGSEGV, SignalHandler);
// 	std::signal(SIGTERM, SignalHandler);
// 	std::signal(SIGBREAK, SignalHandler);
// 	std::signal(SIGABRT, SignalHandler);
	
	InitializeMultiThreadedLogging();

	FCoreDelegates::OnPreExit.AddLambda([this]()
		{
			if (MultiThreadedLogDevice)
			{
				MultiThreadedLogDevice->OnPreExit();
			}
			
		});

}

void FUnrealMultiLogModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	ShutdownMultiThreadedLogging();
	FCoreDelegates::OnPreExit.RemoveAll(this);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealMultiLogModule, UnrealMultiLog)