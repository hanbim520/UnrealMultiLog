// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealMultiLog.h"

#define LOCTEXT_NAMESPACE "FUnrealMultiLogModule"

void FUnrealMultiLogModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	InitializeMultiThreadedLogging();
}

void FUnrealMultiLogModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	ShutdownMultiThreadedLogging();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealMultiLogModule, UnrealMultiLog)