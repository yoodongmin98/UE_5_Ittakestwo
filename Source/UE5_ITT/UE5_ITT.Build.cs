// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE5_ITT : ModuleRules
{
	public UE5_ITT(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput" });
    }
}
