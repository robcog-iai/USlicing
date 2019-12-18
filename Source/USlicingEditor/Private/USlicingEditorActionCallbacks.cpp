// Copyright 2017, Institute for Artificial Intelligence

#include "USlicingEditorActionCallbacks.h"
#include "SlicingComponent.h"
#include "SlicingBladeComponent.h"
#include "SlicingTipComponent.h"
#include "SlicingDebugVisualComponent.h"

#include "Core.h"
#include "Editor.h"

#include "Components/BoxComponent.h"
#include "Engine/Selection.h"
#include "Engine/StaticMeshActor.h"

#define LOCTEXT_NAMESPACE "FUSlicingEditorModule"

void FUSlicingEditorActionCallbacks::ShowInstructions()
{
	const FText InstructionsTitle =
		LOCTEXT("SlicingInstructions_Title", "Instructions to edit an object to be able to slice");
	const FText InstructionsText = FText::FromString(
		FString("To alter an object to be able to slice other objects, ") +
		FString("the user needs to create multiple Sockets on the position - and with the size of - ") +
		FString("certain areas needed for cutting objects. These sockets need specific names to specify ") +
		FString("the area they are used for.\n\nCutting Blade: ") + USlicingComponent::SocketBladeName.ToString() +
		FString("\nCutting Handle: ") + USlicingComponent::SocketHandleName.ToString() +
		FString("\nCutting Tip: ") + USlicingComponent::SocketTipName.ToString());

	// Display the popup-window
	FMessageDialog* Instructions = new FMessageDialog();
	Instructions->Debugf(InstructionsText, &InstructionsTitle);
}

void FUSlicingEditorActionCallbacks::OnEnableDebugConsoleOutput(bool* bButtonValue)
{
	*bButtonValue = !*bButtonValue;
}

bool FUSlicingEditorActionCallbacks::OnIsEnableDebugConsoleOutputEnabled(bool* bButtonValue)
{
	return *bButtonValue;
}

void FUSlicingEditorActionCallbacks::OnEnableDebugShowComponents(bool* bButtonValue)
{
	*bButtonValue = !*bButtonValue;
}

bool FUSlicingEditorActionCallbacks::OnIsEnableDebugShowComponentsEnabled(bool* bButtonValue)
{
	return *bButtonValue;
}

void FUSlicingEditorActionCallbacks::OnEnableDebugShowPlane(bool* bButtonValue)
{
	*bButtonValue = !*bButtonValue;
}

bool FUSlicingEditorActionCallbacks::OnIsEnableDebugShowPlaneEnabled(bool* bButtonValue)
{
	return *bButtonValue;
}

void FUSlicingEditorActionCallbacks::OnEnableDebugShowTrajectory(bool* bButtonValue)
{
	*bButtonValue = !*bButtonValue;
}

bool FUSlicingEditorActionCallbacks::OnIsEnableDebugShowTrajectoryEnabled(bool* bButtonValue)
{
	return *bButtonValue;
}

void FUSlicingEditorActionCallbacks::MakeCuttingObjects()
{
	// Create the BoxComponents for every selected StaticMeshActor
	for (AStaticMeshActor* StaticMeshActor : FUSlicingEditorActionCallbacks::GetSelectedStaticMeshActors())
	{
		UStaticMeshComponent* StaticMesh = StaticMeshActor->GetStaticMeshComponent();

		FUSlicingEditorActionCallbacks::AddHandleComponent(StaticMesh);
		FUSlicingEditorActionCallbacks::AddBladeComponent(StaticMesh);
		FUSlicingEditorActionCallbacks::AddTipComponent(StaticMesh);
		FUSlicingEditorActionCallbacks::AddDebugVisualComponent(StaticMeshActor);
	}
}

void FUSlicingEditorActionCallbacks::MakeCuttableObjects()
{
	for (AStaticMeshActor* StaticMeshActor : FUSlicingEditorActionCallbacks::GetSelectedStaticMeshActors())
	{
		UStaticMeshComponent* StaticMeshComponent = StaticMeshActor->GetStaticMeshComponent();

		// Make the actor identifiable to the cutting object
		if (!StaticMeshComponent->ComponentTags.Contains(USlicingComponent::TagCuttable))
		{
			StaticMeshComponent->ComponentTags.Add(USlicingComponent::TagCuttable);
			StaticMeshComponent->ComponentTags.Add(FName("Resistance"));
			StaticMeshComponent->ComponentTags.Add(FName("0"));
		}
		
		// Let the cutting object go through the actor
		StaticMeshComponent->SetGenerateOverlapEvents(true);

		// Enable physics, as otherwise the object won't be cuttable
		StaticMeshComponent->SetSimulatePhysics(true);
		StaticMeshActor->SetMobility(EComponentMobility::Movable);

		// Add new materialslot for potential inside cut material
		if (StaticMeshComponent->GetStaticMesh()->GetMaterialIndex("InsideCutMaterial") == -1)
		{
			StaticMeshComponent->GetStaticMesh()->Modify();
			
			FStaticMaterial InnerStaticMaterial = FStaticMaterial();
			InnerStaticMaterial.MaterialSlotName = "InsideCutMaterial";
			StaticMeshComponent->GetStaticMesh()->StaticMaterials.Add(InnerStaticMaterial);

			StaticMeshComponent->GetStaticMesh()->PostEditChange();
		}

		// Add the custom resistance property
		// ...
	}
}

TArray<AStaticMeshActor*> FUSlicingEditorActionCallbacks::GetSelectedStaticMeshActors()
{
	TArray<AStaticMeshActor*> SelectedStaticMeshActors;
	GEditor->GetSelectedActors()->GetSelectedObjects<AStaticMeshActor>(SelectedStaticMeshActors);

	if (SelectedStaticMeshActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Slicing-Plugin Error: No StaticMeshActor was selected."));
		return TArray<AStaticMeshActor*>();
	}

	return SelectedStaticMeshActors;
}

void FUSlicingEditorActionCallbacks::AddBoxComponent(UStaticMeshComponent* StaticMesh, UBoxComponent* BoxComponent, FName SocketName, FName CollisionProfileName, bool bGenerateOverlapEvents)
{
	float SocketToBoxScale = 3.5;

	// Makes the added component editable in the editor and saveable in the level
	BoxComponent->GetAttachmentRootActor()->AddInstanceComponent(BoxComponent);

	BoxComponent->RegisterComponent();
	BoxComponent->AttachToComponent(StaticMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	BoxComponent->SetWorldLocation(StaticMesh->GetSocketLocation(SocketName));
	BoxComponent->SetBoxExtent(FVector(1, 1, 1) * SocketToBoxScale);
	BoxComponent->SetCollisionProfileName(CollisionProfileName);
	BoxComponent->SetGenerateOverlapEvents(bGenerateOverlapEvents);
	BoxComponent->bMultiBodyOverlap = true;
}

void FUSlicingEditorActionCallbacks::AddHandleComponent(UStaticMeshComponent* StaticMesh)
{
	if (!StaticMesh->DoesSocketExist(USlicingComponent::SocketHandleName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Slicing-Plugin Error: Socket for the Handle does not exist"));
		return;
	}

	UBoxComponent* HandleComponent =
		NewObject<UBoxComponent>(StaticMesh, USlicingComponent::SocketHandleName);

	FUSlicingEditorActionCallbacks::AddBoxComponent(
		StaticMesh,
		HandleComponent,
		USlicingComponent::SocketHandleName,
		FName("BlockAll"),
		false);
}

void FUSlicingEditorActionCallbacks::AddBladeComponent(UStaticMeshComponent* StaticMesh)
{
	if (!StaticMesh->DoesSocketExist(USlicingComponent::SocketBladeName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Slicing-Plugin Error: Socket for the Blade does not exist"));
		return;
	}

	USlicingBladeComponent* BladeComponent =
		NewObject<USlicingBladeComponent>(StaticMesh, USlicingComponent::SocketBladeName);

	FUSlicingEditorActionCallbacks::AddBoxComponent(
		StaticMesh,
		BladeComponent,
		USlicingComponent::SocketBladeName,
		FName("OverlapAll"),
		true);
}

void FUSlicingEditorActionCallbacks::AddTipComponent(UStaticMeshComponent* StaticMesh)
{
	if (!StaticMesh->DoesSocketExist(USlicingComponent::SocketTipName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Slicing-Plugin Error: Socket for the Tip does not exist"));
		return;
	}

	USlicingTipComponent* TipComponent =
		NewObject<USlicingTipComponent>(StaticMesh, USlicingComponent::SocketTipName);

	FUSlicingEditorActionCallbacks::AddBoxComponent(
		StaticMesh,
		TipComponent,
		USlicingComponent::SocketTipName,
		FName("OverlapAll"),
		true);
}

void FUSlicingEditorActionCallbacks::AddDebugVisualComponent(AStaticMeshActor* StaticMesh)
{
	USlicingDebugVisualComponent* DebugComponent = NewObject<USlicingDebugVisualComponent>(StaticMesh);
	StaticMesh->AddInstanceComponent(DebugComponent);

	DebugComponent->RegisterComponent();
}

#undef LOCTEXT_NAMESPACE