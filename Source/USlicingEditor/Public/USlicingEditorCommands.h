// Copyright 2017, Institute for Artificial Intelligence

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "USlicingEditorStyle.h"

class FUSlicingEditorCommands: public TCommands<FUSlicingEditorCommands>
{
public:
	FUSlicingEditorCommands(): TCommands<FUSlicingEditorCommands>(
		TEXT("USlicing"), // Context name for fast lookup
		NSLOCTEXT("Contexts", "USlicing", "Slicing Plugin"), // Localized context name
		NAME_None, // Parent
		FUSlicingEditorStyle::GetStyleSetName() // Icon Style Set
		){}

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> ShowInstructions;
	TSharedPtr<FUICommandInfo> EnableDebugConsoleOutput;
	TSharedPtr<FUICommandInfo> EnableDebugShowComponents;
	TSharedPtr<FUICommandInfo> EnableDebugShowPlane;
	TSharedPtr<FUICommandInfo> EnableDebugShowTrajectory;
	TSharedPtr<FUICommandInfo> MakeCuttingObjects;
	TSharedPtr<FUICommandInfo> MakeCuttableObjects;
};