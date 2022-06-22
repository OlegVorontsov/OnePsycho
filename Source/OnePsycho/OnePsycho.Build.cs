// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OnePsycho : ModuleRules
{
	public OnePsycho(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule" });
    }
}
