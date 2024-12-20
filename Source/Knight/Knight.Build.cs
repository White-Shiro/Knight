// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using UnrealBuildTool;
using System.IO;

public class Knight : ModuleRules
{
	public Knight(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"ControlRig",
			"RigVM",
			"MotionTrajectory",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"EnhancedInput",
			"AITestSuite",
			"AnimGraphRuntime",
			"Niagara",
			"AIModule",
			"NavigationSystem",
			"GameplayTasks",
			"MotionWarping"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

	}
}
