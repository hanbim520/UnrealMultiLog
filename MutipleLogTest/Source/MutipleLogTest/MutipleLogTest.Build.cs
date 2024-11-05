// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MutipleLogTest : ModuleRules
{
	public MutipleLogTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		bEnableExceptions = true;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UnrealMultiLog" });
	}
}
