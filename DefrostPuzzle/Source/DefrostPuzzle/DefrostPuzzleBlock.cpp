// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefrostPuzzleBlock.h"
#include "DefrostPuzzleBlockGrid.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"

const float ADefrostPuzzleBlock::BlockSize = 10.f;

ADefrostPuzzleBlock::ADefrostPuzzleBlock()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> BlockMesh;
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> RockMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> RockMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> FrozenMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> HardFrozenMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> MeltedMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> OrangeMaterial;
		FConstructorStatics()
			: BlockMesh(TEXT("/Game/Puzzle/Meshes/SM_PuzzleCube.SM_PuzzleCube"))
			, RockMesh(TEXT("/Game/Puzzle/Rock/SM_Rock_A.SM_Rock_A"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/M_CubeBase.M_CubeBase"))
			, RockMaterial(TEXT("/Game/Puzzle/Meshes/MI_CubeRock.MI_CubeRock"))
			, FrozenMaterial(TEXT("/Game/Puzzle/Meshes/MI_CubeFrozen.MI_CubeFrozen"))
			, HardFrozenMaterial(TEXT("/Game/Puzzle/Meshes/MI_CubeFrozen.MI_CubeFrozen"))
			, MeltedMaterial(TEXT("/Game/Puzzle/Meshes/MI_CubeOrange.MI_CubeOrange"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/MI_CubeBlue.MI_CubeBlue"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/MI_CubeOrange.MI_CubeOrange"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	const float puzzleCubeMeshSize = 256.f;
	const float scale = BlockSize / puzzleCubeMeshSize;

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.BlockMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(1.f * scale,1.f * scale, 1.f * scale));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,0.f));
	//BlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	//BlockMesh->OnClicked.AddDynamic(this, &ADefrostPuzzleBlock::BlockClicked);
	//BlockMesh->OnInputTouchBegin.AddDynamic(this, &ADefrostPuzzleBlock::OnFingerPressedBlock);

	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
	RockMaterial = ConstructorStatics.RockMaterial.Get();
	FrozenMaterial = ConstructorStatics.FrozenMaterial.Get();
	HardFrozenMaterial = ConstructorStatics.HardFrozenMaterial.Get();
	MeltedMaterial = ConstructorStatics.MeltedMaterial.Get();
	RockMesh = ConstructorStatics.RockMesh.Get();
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

void ADefrostPuzzleBlock::ChangeRockMesh()
{
	BlockMesh->SetStaticMesh(RockMesh);
	const float scale = 0.01f;
	BlockMesh->SetRelativeScale3D(FVector(1.f * scale, 1.f * scale, 1.f * scale));
}

EBlockType ADefrostPuzzleBlock::GetBlockType() const
{
	return BlockType;
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
		//BlockMesh->SetMaterial(0, BlueMaterial);
		BlockMesh->SetMaterial(0, FrozenMaterial);
	}
}
