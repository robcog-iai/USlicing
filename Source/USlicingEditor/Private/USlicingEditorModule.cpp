// Copyright 2017, Institute for Artificial Intelligence

#include "USlicingEditorModule.h"
#include "USlicingEditorStyle.h"
#include "USlicingEditorCommands.h"
#include "USlicingEditorActionCallbacks.h"
#include "USlicingLogicModule.h"

#include "LevelEditor.h"
#include "StaticMeshEditorModule.h"
#include "Engine.h"

#define LOCTEXT_NAMESPACE "FUSlicingEditorModule"

// This code will execute after your module is loaded into memory; the exact timing is specified in the
// .uplugin file per-module
void FUSlicingEditorModule::StartupModule()
{
	/* Initialize the appearance of the UI buttons */
	FUSlicingEditorStyle::Initialize();
	FUSlicingEditorStyle::ReloadTextures();
	
	InitializeUIButtons();
	CreateInstructionsButton();
	CreateDebugButtons();
}

// This function may be called during shutdown to clean up the module.
void FUSlicingEditorModule::ShutdownModule()
{
	FUSlicingEditorStyle::Shutdown();

	FUSlicingEditorCommands::Unregister();
}

// Creates the menu-entries & buttons in the staticmesheditor
void FUSlicingEditorModule::InitializeUIButtons()
{
	FUSlicingEditorCommands::Register();

	PluginCommandList = MakeShareable(new FUICommandList);
	const FUSlicingEditorCommands& Commands = FUSlicingEditorCommands::Get();

	PluginCommandList->MapAction(
		Commands.ShowInstructions,
		FExecuteAction::CreateStatic(&FUSlicingEditorActionCallbacks::ShowInstructions),
		FCanExecuteAction()
	);

	//* Needed for the debug option booleans
	FUSlicingLogicModule& SlicingLogicModule =
		FModuleManager::Get().LoadModuleChecked<FUSlicingLogicModule>("USlicingLogic");

	PluginCommandList->MapAction(
		Commands.EnableDebugConsoleOutput,
		FExecuteAction::CreateStatic(&FUSlicingEditorActionCallbacks::OnEnableDebugConsoleOutput, &SlicingLogicModule.bEnableDebugConsoleOutput),
		FCanExecuteAction(),
		FIsActionChecked::CreateStatic(&FUSlicingEditorActionCallbacks::OnIsEnableDebugConsoleOutputEnabled, &SlicingLogicModule.bEnableDebugConsoleOutput)
	);
	PluginCommandList->MapAction(
		Commands.EnableDebugShowComponents,
		FExecuteAction::CreateStatic(&FUSlicingEditorActionCallbacks::OnEnableDebugShowComponents, &SlicingLogicModule.bEnableDebugShowComponents),
		FCanExecuteAction(),
		FIsActionChecked::CreateStatic(&FUSlicingEditorActionCallbacks::OnIsEnableDebugShowComponentsEnabled, &SlicingLogicModule.bEnableDebugShowComponents)
	);
	PluginCommandList->MapAction(
		Commands.EnableDebugShowPlane,
		FExecuteAction::CreateStatic(&FUSlicingEditorActionCallbacks::OnEnableDebugShowPlane, &SlicingLogicModule.bEnableDebugShowPlane),
		FCanExecuteAction(),
		FIsActionChecked::CreateStatic(&FUSlicingEditorActionCallbacks::OnIsEnableDebugShowPlaneEnabled, &SlicingLogicModule.bEnableDebugShowPlane)
	);
	PluginCommandList->MapAction(
		Commands.EnableDebugShowTrajectory,
		FExecuteAction::CreateStatic(&FUSlicingEditorActionCallbacks::OnEnableDebugShowTrajectory, &SlicingLogicModule.bEnableDebugShowTrajectory),
		FCanExecuteAction(),
		FIsActionChecked::CreateStatic(&FUSlicingEditorActionCallbacks::OnIsEnableDebugShowTrajectoryEnabled, &SlicingLogicModule.bEnableDebugShowTrajectory)
	);
	PluginCommandList->MapAction(
		Commands.MakeCuttingObjects,
		FExecuteAction::CreateStatic(&FUSlicingEditorActionCallbacks::MakeCuttingObjects)
	);
	PluginCommandList->MapAction(
		Commands.MakeCuttableObjects,
		FExecuteAction::CreateStatic(&FUSlicingEditorActionCallbacks::MakeCuttableObjects)
	);
}

// Adds a toolbar button in the static mesh editor to instruct the user how to create the areas
// needed to make the current object be able to cut other objects
void FUSlicingEditorModule::CreateInstructionsButton()
{
	IStaticMeshEditorModule& StaticMeshEditorModule =
		FModuleManager::Get().LoadModuleChecked<IStaticMeshEditorModule>("StaticMeshEditor");

	// Add toolbar entry
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension(
		"Command",
		EExtensionHook::After,
		PluginCommandList,
		FToolBarExtensionDelegate::CreateRaw(this, &FUSlicingEditorModule::AddInstructionsButton)
	);
	StaticMeshEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

// Creates the buttons to enable debug options in the level editor toolbar
void FUSlicingEditorModule::CreateDebugButtons()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension(
		"Settings",
		EExtensionHook::After,
		PluginCommandList,
		FToolBarExtensionDelegate::CreateRaw(this, &FUSlicingEditorModule::AddDebugOptions)
	);
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

// Creates the menu-entries for the dropdown-menu of the slicing toolbar menu in the level editor
TSharedRef<SWidget> FUSlicingEditorModule::CreateDebugOptionMenu()
{
	FMenuBuilder Builder(false, PluginCommandList.ToSharedRef());

	const FUSlicingEditorCommands& Commands = FUSlicingEditorCommands::Get();
	Builder.BeginSection("SlicingDebugOptions");
	{
		Builder.AddMenuEntry(Commands.EnableDebugConsoleOutput);
		Builder.AddMenuEntry(Commands.EnableDebugShowComponents);
		Builder.AddMenuEntry(Commands.EnableDebugShowPlane);
		Builder.AddMenuEntry(Commands.EnableDebugShowTrajectory);
		Builder.AddMenuEntry(Commands.MakeCuttingObjects);
		Builder.AddMenuEntry(Commands.MakeCuttableObjects);
	}
	Builder.EndSection();

	return Builder.MakeWidget();
}

void FUSlicingEditorModule::AddInstructionsButton(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(
		FUSlicingEditorCommands::Get().ShowInstructions,
		NAME_None,
		LOCTEXT("SlicingInstructionsButton", "Show Slicing Instructions"),
		LOCTEXT("SlicingInstructionsButton_ToolTip", "Show Instructions for the creation of slicing objects"),
		FSlateIcon(FUSlicingEditorStyle::GetStyleSetName(), "USlicingEditor.ShowInstructions")
	);
}

void FUSlicingEditorModule::AddDebugOptions(FToolBarBuilder& Builder)
{
	FUSlicingEditorCommands SlicingEditorCommands;
	
	Builder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FUSlicingEditorModule::CreateDebugOptionMenu),
		LOCTEXT("SlicingDebugToolbar", "Slicing Options"),
		LOCTEXT("SlicingDebugToolbar_ToolTip", "Slicing plugin debug options"),
		FSlateIcon(FUSlicingEditorStyle::GetStyleSetName(), "USlicingEditor.DebugOptionToolBar"),
		false
	);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUSlicingEditorModule, USlicingEditor)