// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyCarsProject : ModuleRules
{
	public MyCarsProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "ChaosVehicles", "PhysicsCore" , "AIModule", "Landscape"});
	}
}
 