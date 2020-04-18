// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShadowSong : ModuleRules
{
	public ShadowSong(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		//PrivatePCHHeaderFile = "ShadowSong.h";

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { 
			"Slate", "SlateCore",
			"GameplayAbilities", "GameplayTags", "GameplayTasks" 
		});

		PublicIncludePaths.AddRange(new string[]
		{
			"ShadowSong/Public/Ability",
			"ShadowSong/Public/Actor",
			"ShadowSong/Public/Character",
			"ShadowSong/Public/Item",
			"ShadowSong/Public/Player"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
