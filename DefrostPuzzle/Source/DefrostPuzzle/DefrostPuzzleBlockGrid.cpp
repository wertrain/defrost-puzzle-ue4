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
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	ScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreText0"));
	ScoreText->SetRelativeLocation(FVector(200.f,0.f,0.f));
	ScoreText->SetRelativeRotation(FRotator(90.f,0.f,0.f));
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "TURN: {0}"), FText::AsNumber(0)));
	ScoreText->SetupAttachment(DummyRoot);

	// Set defaults
	Width = 20;
	Height = 20;
	BlockSpacing = 300.f;
	PuzzleGoalPiece = nullptr;
	Field = std::make_unique<game::Field>();
	Sequence = std::make_unique<SequenceNop>(this);
}

void ADefrostPuzzleBlockGrid::UpdatePuzzlePiecesMesh()
{
	const float blockWidth = ADefrostPuzzleBlock::BlockSize, blockHeighg = ADefrostPuzzleBlock::BlockSize;
	const int32 amountWidth = (Width * blockWidth) * .5f, amountHeight = (Height * blockHeighg) * .5f;

	for (int index = 0; index < PuzzlePieces.Num(); ++index)
	{
		auto piece = PiecePositions[index];
		const float XOffset = piece.x * BlockSpacing;
		const float YOffset = piece.y * BlockSpacing;

		const FVector PieceLocation = FVector(amountHeight - YOffset, -amountWidth + XOffset, ADefrostPuzzleBlock::BlockSize * .5f) + GetActorLocation();

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

	const float blockWidth = ADefrostPuzzleBlock::BlockSize, blockHeighg = ADefrostPuzzleBlock::BlockSize;
	const int32 amountWidth = (Width * blockWidth) * .5f, amountHeight = (Height * blockHeighg) * .5f;

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
					NewBlock->ChangeRockMesh();
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

	std::vector<game::Field::Position> pieces;
	Field->PutPieces(pieces, 4);
	PiecePositions.Reserve(pieces.size());
	DefaultPiecePositions.Reserve(pieces.size());
	for (auto& piece : pieces)
	{
		PiecePositions.Add(piece);
		DefaultPiecePositions.Add(piece);
	}

	// Spawn Pieces
	for (auto& piece : PiecePositions)
	{
		const float XOffset = piece.x * BlockSpacing;
		const float YOffset = piece.y * BlockSpacing;
		
		ADefrostPuzzlePiece* NewPiece = GetWorld()->SpawnActor<ADefrostPuzzlePiece>(FVector::ZeroVector, FRotator(0, 0, 0));
		NewPiece->SetPieceType(EPieceType::Sub);
		PuzzlePieces.Add(NewPiece);

		if (auto* staticMesh = NewPiece->GetPieceMesh())
		{
			staticMesh->OnClicked.AddDynamic(this, &ADefrostPuzzleBlockGrid::PieceMeshClicked);
		}
		const FVector PieceLocation = FVector(amountHeight - YOffset, -amountWidth + XOffset, ADefrostPuzzleBlock::BlockSize * .5f) + GetActorLocation();
		NewPiece->SetActorLocation(PieceLocation);
	}
	PuzzlePieces[0]->SetPieceType(EPieceType::Main);

	auto goalPos = Field->GetGoalPosition();
	PuzzleGoalPiece = GetWorld()->SpawnActor<ADefrostPuzzlePiece>(
		FVector(amountHeight - (goalPos.y * BlockSpacing), -amountWidth + (goalPos.x * BlockSpacing), ADefrostPuzzleBlock::BlockSize * 0.5f) + GetActorLocation(), 
		FRotator(0, 90, 0));
	PuzzleGoalPiece->SetPieceType(EPieceType::Goal);
	PuzzleGoalPiece->SetChildIdle(true);

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

void ADefrostPuzzleBlockGrid::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Sequence->Update(DeltaSeconds);
}

void ADefrostPuzzleBlockGrid::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

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

FText ADefrostPuzzleBlockGrid::GetFieldCode() const
{
	std::string serialized;
	Field->Serialize(serialized);

	return FText::FromString(serialized.c_str());
}

void ADefrostPuzzleBlockGrid::AddScore()
{
	// Increment score
	SetScore(Score+1);
}

void ADefrostPuzzleBlockGrid::ResetScore()
{
	SetScore(0);
}

void ADefrostPuzzleBlockGrid::SetScore(const int NewScore)
{
	Score = NewScore;

	// Update text
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "TURN: {0}"), FText::AsNumber(Score)));
}

int32 ADefrostPuzzleBlockGrid::GetScore() const
{
	return Score;
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

std::pair<int32, int32> ADefrostPuzzleBlockGrid::GetPuzzlePiecePositionByIndex(const int PieceIndex) const
{
	return std::pair<int32, int32>(PiecePositions[PieceIndex].x, PiecePositions[PieceIndex].y);
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

const TArray<game::Field::Position>& ADefrostPuzzleBlockGrid::GetPieces() const
{
	return PiecePositions;
}

void ADefrostPuzzleBlockGrid::SetHighlightBlock(const int PieceIndex)
{
	auto& piece = PiecePositions[PieceIndex];
	
	ResetHighlightAll();

	if (auto* block = GetPuzzleBlock(piece.x, piece.y))
	{
		block->Highlight(true);
	}	
}

int ADefrostPuzzleBlockGrid::SetHighlightDirection(const int PieceIndex, const EPuzzleDirection Direction)
{
	if (PiecePositions.Num() <= PieceIndex)
	{
		return 0;
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
	return 0;
#else

	ResetHighlightAll();

	std::vector<ADefrostPuzzleBlock*> blocks;
	bool isGoal = false;
	ADefrostPuzzleBlockGrid::GetPuzzleBlockLine(PieceIndex, Direction, blocks, isGoal);

	for (auto& block : blocks)
	{
		block->Highlight(true);
	}

	return static_cast<int>(blocks.size());
#endif
}

void ADefrostPuzzleBlockGrid::ResetHighlightAll()
{
	for (auto& block : PuzzleBlocks)
	{
		if (block->GetBlockType() != EBlockType::Rock)
		{
			block->Highlight(false);
		}
	}
}

void ADefrostPuzzleBlockGrid::SetPieceDirection(const int PieceIndex, const EPuzzleDirection Direction)
{
	if (!Sequence->CanPlayerControl())
	{
		return;
	}

	float eulerZ[] = { -90.0f, 180.0f, 0.0f, 90.0f };
	PuzzlePieces[PieceIndex]->SetActorRotation(FQuat::MakeFromEuler(FVector(0, 0, eulerZ[static_cast<int8>(Direction)])));
}

void ADefrostPuzzleBlockGrid::SetPiecePosition(const int PieceIndex, const std::pair<int32, int32> Position)
{
	PiecePositions[PieceIndex] = game::Field::Position(std::get<0>(Position), std::get<1>(Position));
}

bool ADefrostPuzzleBlockGrid::MovePiece(const int PieceIndex, const EPuzzleDirection Direction)
{
	std::vector<ADefrostPuzzleBlock*> blocks;
	bool isGoal = false;
	auto pair = ADefrostPuzzleBlockGrid::GetPuzzleBlockLine(PieceIndex, Direction, blocks, isGoal);

	auto target = game::Field::Position(std::get<0>(pair), std::get<1>(pair));

	auto* sequence = NextSequence<SequenceMovePiece>();
	sequence->SetTarget(PuzzlePieces[PieceIndex], PiecePositions[PieceIndex], target);
	
	PiecePositions[PieceIndex] = target;

	return (PieceIndex == 0) && isGoal;
}

void ADefrostPuzzleBlockGrid::ResetPieces()
{
	for (int index = 0; index < DefaultPiecePositions.Num(); ++index)
	{
		PiecePositions[index] = DefaultPiecePositions[index];
	}
}

bool ADefrostPuzzleBlockGrid::IsGoal(const int Index) const
{
	return Field->GetCell(Index % Width, Index / Width) == game::Field::CellType::Goal;
}

bool ADefrostPuzzleBlockGrid::CheckGoal(const game::Field::Position& Position) const
{
	if (Position.x - 1 >= 0)
	{
		if (Field->GetCell(Position.x - 1, Position.y) == game::Field::CellType::Goal)
		{
			return true;
		}
	}

	if (Position.x + 1 < Width)
	{
		if (Field->GetCell(Position.x + 1, Position.y) == game::Field::CellType::Goal)
		{
			return true;
		}
	}

	if (Position.y - 1 >= 0)
	{
		if (Field->GetCell(Position.x, Position.y - 1) == game::Field::CellType::Goal)
		{
			return true;
		}
	}

	if (Position.y + 1 < Height)
	{
		if (Field->GetCell(Position.x, Position.y + 1) == game::Field::CellType::Goal)
		{
			return true;
		}
	}

	return Field->GetCell(Position.x, Position.y) == game::Field::CellType::Goal;
}

int ADefrostPuzzleBlockGrid::IsOnPiece(const class ADefrostPuzzleBlock* Block) const
{
	const auto& position = GetPuzzleBlockPosition(Block);

	for (int32 index = 0, size = PiecePositions.Num(); index < size; ++index)
	{
		if (PiecePositions[index].x == std::get<0>(position) && PiecePositions[index].y == std::get<1>(position))
		{
			return index;
		}
	}

	return -1;
}

bool ADefrostPuzzleBlockGrid::CanPlayerControllable() const
{
	return Sequence->CanPlayerControl();
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
		for (int32 index = 0; index < PiecePositions.Num(); ++index)
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

//-------------------------------------------------------------------------------------------------

ADefrostPuzzleBlockGrid::SequenceMovePiece::SequenceMovePiece(ADefrostPuzzleBlockGrid* p)
	: PuzzleBlockGridSequenceBase(p)
	, TargetPiece()
	, StartPieceLocation()
	, EndPieceLocation()
	, CurrentTime(.0f)
	, TargetTime(.0f)
	, InSequence(ADefrostPuzzleBlockGrid::SequenceMovePiece::InSequenceId::InAnim)
	, AnimTime(.0f)
	, IsGoal(false)
{

}

void ADefrostPuzzleBlockGrid::SequenceMovePiece::Update(float DeltaSeconds)
{
	switch (InSequence)
	{
	case ADefrostPuzzleBlockGrid::SequenceMovePiece::InSequenceId::InAnim:
		Update_InAnim(DeltaSeconds);
		break;
	case ADefrostPuzzleBlockGrid::SequenceMovePiece::InSequenceId::Sliding:
		Update_Sliding(DeltaSeconds);
		break;
	case ADefrostPuzzleBlockGrid::SequenceMovePiece::InSequenceId::OutAnim:
		Update_OutAnim(DeltaSeconds);
		break;
	default:
		break;
	}
}

void ADefrostPuzzleBlockGrid::SequenceMovePiece::SetTarget(ADefrostPuzzlePiece* Piece, const game::Field::Position& Start, const game::Field::Position& Goal)
{
	const float blockWidth = ADefrostPuzzleBlock::BlockSize, blockHeighg = ADefrostPuzzleBlock::BlockSize;
	const int32 amountWidth = (Owner->Width * blockWidth) * .5f, amountHeight = (Owner->Height * blockHeighg) * .5f;

	TargetPiece = Piece;

	StartPieceLocation = Piece->GetActorLocation();
	
	EndPieceLocation = FVector(
		amountHeight - (Goal.y * Owner->BlockSpacing),
		-amountWidth + (Goal.x * Owner->BlockSpacing),
		ADefrostPuzzleBlock::BlockSize * .5f
	) + Owner->GetActorLocation();

	float distance = FVector::Distance(EndPieceLocation, StartPieceLocation);

	CurrentTime = 0;
	TargetTime = FMath::Clamp(1.f / (distance / (ADefrostPuzzleBlock::BlockSize)), 0.5f, 1.0f);
	TargetPiece->SetSliding(true);

	IsGoal = Owner->GetPuzzlePieceIndex(TargetPiece) == 0 && Owner->CheckGoal(Goal);
}

bool ADefrostPuzzleBlockGrid::SequenceMovePiece::IsMoveEnd() const
{
	return CurrentTime >= TargetTime;
}

void ADefrostPuzzleBlockGrid::SequenceMovePiece::Update_InAnim(float DeltaSeconds)
{
	if ((AnimTime += DeltaSeconds) >= 0.5f)
	{
		InSequence = InSequenceId::Sliding;
		AnimTime = 0;
	}
}

void ADefrostPuzzleBlockGrid::SequenceMovePiece::Update_Sliding(float DeltaSeconds)
{
	if ((CurrentTime += DeltaSeconds) >= TargetTime)
	{
		CurrentTime = TargetTime;

		InSequence = InSequenceId::OutAnim;
		TargetPiece->SetSliding(false);
	}

	if (TargetTime > CurrentTime)
	{
		float rate = 1.0f - (TargetTime - CurrentTime) / TargetTime;
		TargetPiece->SetActorLocation(
			FMath::Lerp(StartPieceLocation, EndPieceLocation, rate)
		);
	}
}

void ADefrostPuzzleBlockGrid::SequenceMovePiece::Update_OutAnim(float DeltaSeconds)
{
	if ((AnimTime += DeltaSeconds) >= 0.5f)
	{
		Owner->UpdatePuzzlePiecesMesh();
		AnimTime = 0;

		if (IsGoal)
		{
			Owner->ResetHighlightAll();
			Owner->OnGameFinished();
			Owner->NextSequence<SequenceFinished>();
		}
		else
		{
			Owner->NextSequence<SequenceNop>();
		}
	}
}

void ADefrostPuzzleBlockGrid::SequenceFinished::Update(float DeltaSeconds)
{

}

#undef LOCTEXT_NAMESPACE
