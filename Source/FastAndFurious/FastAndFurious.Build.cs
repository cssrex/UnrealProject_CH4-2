// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FastAndFurious : ModuleRules
{
    public FastAndFurious(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "ChaosVehicles",
            "PhysicsCore",
            "UMG",
            "Slate",
            "SlateCore"
        });

        PublicIncludePaths.AddRange(new string[] { "FastAndFurious" });
    }
}
