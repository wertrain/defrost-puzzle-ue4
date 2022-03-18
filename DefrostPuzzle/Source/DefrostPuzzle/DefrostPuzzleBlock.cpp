// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefrostPuzzleBlock.h"
#include "DefrostPuzzleBlockGrid.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"

ADefrostPuzzleBlock::ADefrostPuzzleBlock()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> RockMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> FrozenMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> HardFrozenMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> MeltedMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> OrangeMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, RockMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.RockMaterial"))
			, FrozenMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.FrozenMaterial"))
			, HardFrozenMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.FrozenMaterial"))
			, MeltedMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.OrangeMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	const float scale = .1f;

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(1.f * scale,1.f * scale,0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,25.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	BlockMesh->OnClicked.AddDynamic(this, &ADefrostPuzzleBlock::BlockClicked);
	BlockMesh->OnInputTouchBegin.AddDynamic(this, &ADefrostPuzzleBlock::OnFingerPressedBlock);

	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
}

void ADefrostPuzzleBlock::BlockClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked)
{
	HandleClicked();
}


void ADefrostPuzzleBlock::OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	HandleClicked();
}

void ADefrostPuzzleBlock::SetBlockType(const EBlockType Type)
{
	BlockType = Type;

	UMaterialInstance* TypeToMaterial[static_cast<uint8>(EBlockType::Num)] =
	{
		RockMaterial,		// Rock
		FrozenMaterial,		// Frozen
		HardFrozenMaterial,	// HardFrozen
		MeltedMaterial,		// Melted
	};

	// Change material
	BlockMesh->SetMaterial(0, TypeToMaterial[static_cast<uint8>(Type)]);
}

void ADefrostPuzzleBlock::HandleClicked()
{
	// Check we are not already active
	if (!bIsActive)
	{
		bIsActive = true;

		// Change material
		BlockMesh->SetMaterial(0, OrangeMaterial);

		// Tell the Grid
		if (OwningGrid != nullptr)
		{
			OwningGrid->AddScore();
		}
	}
}

void ADefrostPuzzleBlock::Highlight(bool bOn)
{
	// Do not highlight if the block has already been activated.
	if (bIsActive)
	{
		return;
	}

	if (bOn)
	{
		BlockMesh->SetMaterial(0, BaseMaterial);
	}
	else
	{
		BlockMesh->SetMaterial(0, BlueMaterial);
	}
}
