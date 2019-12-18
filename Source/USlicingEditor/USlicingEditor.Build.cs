// Copyright 2017, Institute for Artificial Intelligence

using UnrealBuildTool;
using System.IO;

public class USlicingEditor: ModuleRules
{
	public USlicingEditor(ReadOnlyTargetRules Target): base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

        // Public dependencies that are statically linked
        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "StaticMeshEditor",
                "USlicingLogic",
                "ProceduralMeshComponent"
            }
		);

        // Private dependencies that are statically linked
        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore"
			}
		);
		
		// Dependencies that are dynamically loaded
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
