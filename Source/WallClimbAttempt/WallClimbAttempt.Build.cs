// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WallClimbAttempt : ModuleRules
{
	public WallClimbAttempt(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"WallClimbAttempt",
			"WallClimbAttempt/Variant_Platforming",
			"WallClimbAttempt/Variant_Platforming/Animation",
			"WallClimbAttempt/Variant_Combat",
			"WallClimbAttempt/Variant_Combat/AI",
			"WallClimbAttempt/Variant_Combat/Animation",
			"WallClimbAttempt/Variant_Combat/Gameplay",
			"WallClimbAttempt/Variant_Combat/Interfaces",
			"WallClimbAttempt/Variant_Combat/UI",
			"WallClimbAttempt/Variant_SideScrolling",
			"WallClimbAttempt/Variant_SideScrolling/AI",
			"WallClimbAttempt/Variant_SideScrolling/Gameplay",
			"WallClimbAttempt/Variant_SideScrolling/Interfaces",
			"WallClimbAttempt/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
