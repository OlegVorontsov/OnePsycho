// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OnePsycho : ModuleRules
{
	public OnePsycho(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay",
			"NavigationSystem",
			"AIModule",
			"PhysicsCore",
			"Slate"
		});

		//явно подключаем необходимые папки проекта
		PublicIncludePaths.AddRange(new string[]
		{
			"OnePsycho/Character",
			"OnePsycho/FuncLibrary",
			"OnePsycho/Game",
			"OnePsycho/Weapon",
		});
	}
}

