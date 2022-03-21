// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefrostPuzzleBlockGrid.h"
#include "DefrostPuzzleBlock.h"
#include "DefrostPuzzlePiece.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"
#include <functional>

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
	Width = 20;
	Height = 20;
	BlockSpacing = 300.f;
	Field = std::make_unique<game::Field>();
}

void ADefrostPuzzleBlockGrid::UpdatePuzzlePiecesMesh()
{
	const float blockWidth = 10.f, blockHeighg = 10.f;
	int32 amountWidth = (Width * blockWidth) * .5f;
	int32 amountHeight = (Height * blockHeighg) * .5f;

	for (int index = 0; index < PuzzlePieces.Num(); ++index)
	{
		auto piece = PiecePositions[index];
		const float XOffset = piece.x * BlockSpacing;
		const float YOffset = piece.y * BlockSpacing;

		const FVector PieceLocation = FVector(amountHeight - YOffset, -amountWidth + XOffset, 40.f) + GetActorLocation();

		PuzzlePieces[index]->SetActorLocation(PieceLocation);
	}
}

void ADefrostPuzzleBlockGrid::BeginPlay()
{
	Super::BeginPlay();

	game::Field::CreateParameter param;
	param.width = Width;
	param.height = Height;
	Field->Create(param);

	const float blockWidth = 10.f, blockHeighg = 10.f;
	int32 amountWidth = (Width * blockWidth) * .5f;
	int32 amountHeight = (Height * blockHeighg) * .5f;

	for (int32 y = 0; y < Height; ++y)
	{
		for (int32 x = 0; x < Width; ++x)
		{
			const float XOffset = x * BlockSpacing;
			const float YOffset = y * BlockSpacing;

			// Make position vector, offset from Grid location
			const FVector BlockLocation = FVector(amountHeight - YOffset, -amountWidth + XOffset, 0.f) + GetActorLocation();

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

				if (auto* staticMesh = NewBlock->GetBlockMesh())
				{
					staticMesh->OnClicked.AddDynamic(this, &ADefrostPuzzleBlockGrid::BlockMeshClicked);
					staticMesh->OnInputTouchBegin.AddDynamic(this, &ADefrostPuzzleBlockGrid::OnFingerPressedBlock);
				}
				PuzzleBlocks.Add(NewBlock);
			}
		}
	}

	Field->PutPieces(PiecePositions, 4);
	DefaultPiecePositions.reserve(PiecePositions.size());
	copy(PiecePositions.begin(), PiecePositions.end(), DefaultPiecePositions.begin());

	// Spawn a Piece
	for (auto& piece : PiecePositions)
	{
		const float XOffset = piece.x * BlockSpacing;
		const float YOffset = piece.y * BlockSpacing;

		const FVector PieceLocation = FVector(amountHeight - YOffset, -amountWidth + XOffset, 40.f) + GetActorLocation();
		ADefrostPuzzlePiece* NewPiece = GetWorld()->SpawnActor<ADefrostPuzzlePiece>(PieceLocation, FRotator(0, 0, 0));
		PuzzlePieces.Add(NewPiece);

		if (auto* staticMesh = NewPiece->GetPieceMesh())
		{
			staticMesh->OnClicked.AddDynamic(this, &ADefrostPuzzleBlockGrid::PieceMeshClicked);
		}
	}


	Field->Dump([&](const game::Field::CellType** cells, const int width, const int height)
	{
		for (int y = 0; y < height; ++y)
		{
			TStringBuilder<256> builder;
			for (int x = 0; x < width; ++x)
			{
				builder.Append('0' + static_cast<uint8_t>(cells[y][x]));
			}
			UE_LOG(LogTemp, Log, TEXT("%s"), builder.ToString());
		}
	});

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

void ADefrostPuzzleBlockGrid::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto& block : PuzzleBlocks)
	{
		block->RemoveFromRoot();
	}
	PuzzleBlocks.Reset(0);

	for (auto& piece : PuzzlePieces)
	{
		piece->RemoveFromRoot();
	}
	PuzzlePieces.Reset(0);
}

void ADefrostPuzzleBlockGrid::AddScore()
{
	// Increment score
	Score++;

	// Update text
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(Score)));
}

ADefrostPuzzleBlock* ADefrostPuzzleBlockGrid::GetPuzzleBlock(const int x, const int y)
{
	const int32 index = (y * Width) + x;
	return PuzzleBlocks[index];
}

ADefrostPuzzleBlock* ADefrostPuzzleBlockGrid::GetPuzzleBlock(const int index)
{
	return PuzzleBlocks[index];
}

int32 ADefrostPuzzleBlockGrid::GetPuzzleBlockIndex(const ADefrostPuzzleBlock* Block) const
{
	for (int32 index = 0, size = Height * Width; index < size; ++index)
	{
		if (PuzzleBlocks[index] == Block)
		{
			return index;
		}
	}
	return -1;
}

std::pair<int32, int32> ADefrostPuzzleBlockGrid::GetPuzzleBlockPosition(const ADefrostPuzzleBlock* Block) const
{
	for (int32 y = 0; y < Height; ++y)
	{
		for (int32 x = 0; x < Width; ++x)
		{
			const int32 index = (y * Width) + x;

			if (PuzzleBlocks[index] == Block)
			{
				return std::pair<int32, int32>(x, y);
			}
		}
	}
	return std::pair<int32, int32>(-1, -1);
}

int ADefrostPuzzleBlockGrid::GetPuzzlePieceIndex(const ADefrostPuzzlePiece* Piece) const
{
	for (int32 index = 0; index < PuzzlePieces.Num(); ++index)
	{
		if (PuzzlePieces[index] == Piece)
		{
			return index;
		}
	}
	return -1;
}

const std::vector<game::Field::Position>& ADefrostPuzzleBlockGrid::GetPieces() const
{
	return PiecePositions;
}

void ADefrostPuzzleBlockGrid::SetHighlightDirection(const int PieceIndex, const EPuzzleDirection Direction)
{
	if (PiecePositions.size() <= PieceIndex)
	{
		return;
	}

#if 0
	auto start = PiecePositions[PieceIndex];
	std::function<bool(int, int)> checkDirection[] =
	{
		[&start](const int px, const int py)
		{
			return px == start.x && py < start.y;
		},
		[&start](const int px, const int py)
		{
			return px < start.x && py == start.y;
		},
		[&start](const int px, const int py)
		{
			return px > start.x && py == start.y;
		},
		[&start](const int px, const int py)
		{
			return px == start.x && py > start.y;
		}
	};

	for (int32 y = 0; y < Height; ++y)
	{
		for (int32 x = 0; x < Width; ++x)
		{
			const int32 index = (y * Width) + x;

			if (PuzzleBlocks[index]->GetBlockType() != EBlockType::Rock)
			{
				PuzzleBlocks[index]->Highlight(checkDirection[static_cast<uint8>(Direction)](x, y));
			}
		}
	}
#else

	for (auto& block : PuzzleBlocks)
	{
		if (block->GetBlockType() != EBlockType::Rock)
		{
			block->Highlight(false);
		}
	}

	std::vector<ADefrostPuzzleBlock*> blocks;
	bool isGoal = false;
	ADefrostPuzzleBlockGrid::GetPuzzleBlockLine(PieceIndex, Direction, blocks, isGoal);

	for (auto& block : blocks)
	{
		block->Highlight(true);
	}
#endif
}

bool ADefrostPuzzleBlockGrid::MovePiece(const int PieceIndex, const EPuzzleDirection Direction)
{
	std::vector<ADefrostPuzzleBlock*> blocks;
	bool isGoal = false;
	auto pair = ADefrostPuzzleBlockGrid::GetPuzzleBlockLine(PieceIndex, Direction, blocks, isGoal);

	PiecePositions[PieceIndex] = game::Field::Position(std::get<0>(pair), std::get<1>(pair));

	return (PieceIndex == 0) && isGoal;
}

void ADefrostPuzzleBlockGrid::ResetPiece()
{
	copy(DefaultPiecePositions.begin(), DefaultPiecePositions.end(), PiecePositions.begin());
}

bool ADefrostPuzzleBlockGrid::IsGoal(const int Index) const
{
	return Field->GetCell(Index % Width, Index / Width) == game::Field::CellType::Goal;
}

void ADefrostPuzzleBlockGrid::AddListener(IDefrostPuzzleBlockGridListener* Listener)
{
	Listeners.AddUnique(Listener);
}

std::pair<int32, int32> ADefrostPuzzleBlockGrid::GetPuzzleBlockLine(const int PieceIndex, const EPuzzleDirection Direction, std::vector<class ADefrostPuzzleBlock*>& OutList, bool& OutIsGoal)
{
	auto start = PiecePositions[PieceIndex];

	auto checkPieces = [&](const int px, const int py)
	{
		for (int32 index = 0; index < PiecePositions.size(); ++index)
		{
			if (PieceIndex != index)
			{
				if (px == PiecePositions[index].x && py == PiecePositions[index].y)
				{
					return true;
				}
			}
		}
		return false;
	};

	switch (Direction)
	{
	case EPuzzleDirection::Up:
		for (int32 y = start.y; y >= 0; --y)
		{
			const int32 index = (y * Width) + start.x;

			OutIsGoal = IsGoal(index);
			if (checkPieces(start.x, y) || PuzzleBlocks[index]->GetBlockType() != EBlockType::Frozen)
			{
				break;
			}
			start.y = y;
			OutList.push_back(PuzzleBlocks[index]);
		}
		break;

	case EPuzzleDirection::Down:
		for (int32 y = start.y; y < Height; ++y)
		{
			const int32 index = (y * Width) + start.x;

			OutIsGoal = IsGoal(index);
			if (checkPieces(start.x, y) || PuzzleBlocks[index]->GetBlockType() != EBlockType::Frozen)
			{
				break;
			}
			start.y = y;
			OutList.push_back(PuzzleBlocks[index]);
		}
		break;

	case EPuzzleDirection::Left:
		for (int32 x = start.x; x >= 0; --x)
		{
			const int32 index = (start.y * Width) + x;

			OutIsGoal = IsGoal(index);
			if (checkPieces(x, start.y) || PuzzleBlocks[index]->GetBlockType() != EBlockType::Frozen)
			{
				break;
			}
			start.x = x;
			OutList.push_back(PuzzleBlocks[index]);
		}
		break;

	case EPuzzleDirection::Right:
		for (int32 x = start.x; x < Width; ++x)
		{
			const int32 index = (start.y * Width) + x;

			OutIsGoal = IsGoal(index);
			if (checkPieces(x, start.y) || PuzzleBlocks[index]->GetBlockType() != EBlockType::Frozen)
			{
				break;
			}
			start.x = x;
			OutList.push_back(PuzzleBlocks[index]);
		}
		break;
	}

	return std::pair<int, int>(start.x, start.y);
}

void ADefrostPuzzleBlockGrid::BlockMeshClicked(UPrimitiveComponent* ClickedComponent, FKey ButtonClicked)
{
	auto* hitBlock = ClickedComponent->GetOwner<ADefrostPuzzleBlock>();

	for (const auto& listener : Listeners)
	{
		listener->OnBlockMeshClicked(hitBlock, GetPuzzleBlockPosition(hitBlock), GetPuzzleBlockIndex(hitBlock));
	}
}

void ADefrostPuzzleBlockGrid::PieceMeshClicked(UPrimitiveComponent* ClickedComponent, FKey ButtonClicked)
{
	auto* hitPiece = ClickedComponent->GetOwner<ADefrostPuzzlePiece>();
	auto hitIndex = GetPuzzlePieceIndex(hitPiece);	

	for (const auto& listener : Listeners)
	{
		listener->OnPieceMeshClicked(hitPiece, std::pair<int32, int32>(
			PiecePositions[hitIndex].x,PiecePositions[hitIndex].y), hitIndex);
	}
}

void ADefrostPuzzleBlockGrid::OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{

}

#undef LOCTEXT_NAMESPACE
