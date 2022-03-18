// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefrostPuzzleBlockGrid.h"
#include "DefrostPuzzleBlock.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"

ADefrostPuzzleBlockGrid::ADefrostPuzzleBlockGrid()
{
	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	ScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreText0"));
	ScoreText->SetRelativeLocation(FVector(200.f,0.f,0.f));
	ScoreText->SetRelativeRotation(FRotator(90.f,0.f,0.f));
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(0)));
	ScoreText->SetupAttachment(DummyRoot);

	// Set defaults
	Size = 3;
	Width = 20;
	Height = 20;
	BlockSpacing = 300.f;
	Field = std::make_unique<game::Field>();
}


void ADefrostPuzzleBlockGrid::BeginPlay()
{
	Super::BeginPlay();


	game::Field::CreateParameter param;
	Field->Create(param);

	for (int32 y = 0; y < Height; ++y)
	{
		for (int32 x = 0; x < Width; ++x)
		{
			const float XOffset = x * BlockSpacing;
			const float YOffset = y * BlockSpacing;

			// Make position vector, offset from Grid location
			const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

			// Spawn a block
			ADefrostPuzzleBlock* NewBlock = GetWorld()->SpawnActor<ADefrostPuzzleBlock>(BlockLocation, FRotator(0, 0, 0));

			// Tell the block about its owner
			if (NewBlock != nullptr)
			{
				NewBlock->OwningGrid = this;

				switch (Field->GetCell(x, y))
				{
				case game::Field::CellType::Block:
					NewBlock->SetBlockType(EBlockType::Rock);
					break;
				case game::Field::CellType::Frozen:
					NewBlock->SetBlockType(EBlockType::Frozen);
					break;
				case game::Field::CellType::HardFrozen:
					NewBlock->SetBlockType(EBlockType::HardFrozen);
					break;
				}
			}
		}
	}

#if 0
	// Number of blocks
	const int32 NumBlocks = Size * Size;

	// Loop to spawn each block
	for(int32 BlockIndex=0; BlockIndex<NumBlocks; BlockIndex++)
	{
		const float XOffset = (BlockIndex/Size) * BlockSpacing; // Divide by dimension
		const float YOffset = (BlockIndex%Size) * BlockSpacing; // Modulo gives remainder

		// Make position vector, offset from Grid location
		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		// Spawn a block
		ADefrostPuzzleBlock* NewBlock = GetWorld()->SpawnActor<ADefrostPuzzleBlock>(BlockLocation, FRotator(0,0,0));

		// Tell the block about its owner
		if (NewBlock != nullptr)
		{
			NewBlock->OwningGrid = this;
		}
	}
#endif
}


void ADefrostPuzzleBlockGrid::AddScore()
{
	// Increment score
	Score++;

	// Update text
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(Score)));
}

#undef LOCTEXT_NAMESPACE
