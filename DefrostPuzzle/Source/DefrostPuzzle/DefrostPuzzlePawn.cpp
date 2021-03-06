// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefrostPuzzlePawn.h"
#include "DefrostPuzzleBlock.h"
#include "DefrostPuzzlePiece.h"
#include "DefrostPuzzleBlockGrid.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ADefrostPuzzlePawn::ADefrostPuzzlePawn(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	PuzzleBlockGrid = nullptr;
	ActivePieceIndex = 0;
	CurrentPieceIndex = 0;
	CurrentPieceDirection = EPuzzleDirection::Up;
	CanMoveCurrentDirection = 0;
	SelectionMode = PuzzleBlockSelectMode::None;
}

void ADefrostPuzzlePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (PuzzleBlockGrid)
	{
		if (PuzzleBlockGrid->CanPlayerControllable())
		{
			switch (SelectionMode)
			{
			case ADefrostPuzzlePawn::PuzzleBlockSelectMode::None:
				break;
			case ADefrostPuzzlePawn::PuzzleBlockSelectMode::Piece:
				PuzzleBlockGrid->SetHighlightBlock(ActivePieceIndex);
				break;
			case ADefrostPuzzlePawn::PuzzleBlockSelectMode::Direction:
				CanMoveCurrentDirection = PuzzleBlockGrid->SetHighlightDirection(CurrentPieceIndex, CurrentPieceDirection) - 1;
				PuzzleBlockGrid->SetPieceDirection(CurrentPieceIndex, CurrentPieceDirection);
				break;
			default:
				break;
			}
		}
		else
		{

		}
	}
	else
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefrostPuzzleBlockGrid::StaticClass(), FoundActors);

		if (FoundActors.Num() > 0)
		{
			PuzzleBlockGrid = Cast<ADefrostPuzzleBlockGrid>(FoundActors[0]);
			PuzzleBlockGrid->AddListener(this);
		}
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UCameraComponent* OurCamera = PC->GetViewTarget()->FindComponentByClass<UCameraComponent>())
			{
				FVector Start = OurCamera->GetComponentLocation();
				FVector End = Start + (OurCamera->GetComponentRotation().Vector() * 8000.0f);
				TraceForBlock(Start, End, true);
			}
		}
		else
		{
			FVector Start, Dir, End;
			PC->DeprojectMousePositionToWorld(Start, Dir);
			End = Start + (Dir * 8000.0f);
			TraceForBlock(Start, End, false);
		}
	}
}

void ADefrostPuzzlePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", EInputEvent::IE_Pressed, this, &ADefrostPuzzlePawn::OnResetVR);
	PlayerInputComponent->BindAction("TriggerClick", EInputEvent::IE_Pressed, this, &ADefrostPuzzlePawn::TriggerClick);
}

void ADefrostPuzzlePawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);

	OutResult.Rotation = FRotator(-90.0f, -90.0f, 0.0f);
}

void ADefrostPuzzlePawn::ResetAllPieces()
{
	PuzzleBlockGrid->ResetPieces();
	PuzzleBlockGrid->UpdatePuzzlePiecesMesh();
	PuzzleBlockGrid->ResetScore();
	PieceCommands.Empty();
}

void ADefrostPuzzlePawn::UndoPiece()
{
	if (PieceCommands.Num() > 0)
	{
		auto command = PieceCommands.Pop();
		UndoRedoCommands.Push(command);

		PuzzleBlockGrid->SetPiecePosition(command.PieceIndex, command.BeforePosition);
		PuzzleBlockGrid->UpdatePuzzlePiecesMesh();
		PuzzleBlockGrid->SetScore(command.BeforeScore);
	}
}

void ADefrostPuzzlePawn::RedoPiece()
{
	if (UndoRedoCommands.Num() > 0)
	{
		auto command = UndoRedoCommands.Pop();
		PieceCommands.Push(command);

		PuzzleBlockGrid->SetPiecePosition(command.PieceIndex, command.AfterPosition);
		PuzzleBlockGrid->UpdatePuzzlePiecesMesh();
		PuzzleBlockGrid->SetScore(command.AfterScore);
	}
}


FText ADefrostPuzzlePawn::GetHandsCode() const
{
	std::string hands;

	for (auto& command : PieceCommands)
	{
		hands.push_back('0' + command.PieceIndex);
		hands.push_back('0' + static_cast<uint8>(command.PieceDirection));
	}

	return FText::FromString(hands.c_str());
}

void ADefrostPuzzlePawn::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ADefrostPuzzlePawn::TriggerClick()
{
	if (CurrentBlockFocus)
	{
		CurrentBlockFocus->HandleClicked();
	}
}

void ADefrostPuzzlePawn::TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers)
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
	if (bDrawDebugHelpers)
	{
		DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red);
		DrawDebugSolidBox(GetWorld(), HitResult.Location, FVector(20.0f), FColor::Red);
	}
#if 0
	if (HitResult.Actor.IsValid())
	{
		ADefrostPuzzleBlock* HitBlock = Cast<ADefrostPuzzleBlock>(HitResult.Actor.Get());
		if (CurrentBlockFocus != HitBlock)
		{
			if (CurrentBlockFocus)
			{
				CurrentBlockFocus->Highlight(false);
			}
			if (HitBlock)
			{
				HitBlock->Highlight(true);
			}
			CurrentBlockFocus = HitBlock;
		}
	}
	else if (CurrentBlockFocus)
	{
		CurrentBlockFocus->Highlight(false);
		CurrentBlockFocus = nullptr;
	}
#else
	if (HitResult.Actor.IsValid())
	{
		if (HitResult.Actor->GetClass() == ADefrostPuzzleBlock::StaticClass())
		{
			ADefrostPuzzleBlock* HitBlock = Cast<ADefrostPuzzleBlock>(HitResult.Actor.Get());
			auto hitPosition = PuzzleBlockGrid->GetPuzzleBlockPosition(HitBlock);
			const int hitX = std::get<0>(hitPosition), hitY = std::get<1>(hitPosition);
			auto piece = PuzzleBlockGrid->GetPieces()[CurrentPieceIndex];

			int onPieceIndex = PuzzleBlockGrid->IsOnPiece(HitBlock);
			if (onPieceIndex >= 0)
			{
				SelectionMode = PuzzleBlockSelectMode::Piece;
				ActivePieceIndex = onPieceIndex;
			}
			else
			{
				if (hitX == piece.x)
				{
					CurrentPieceDirection = (hitY < piece.y) ? EPuzzleDirection::Up : EPuzzleDirection::Down;
				}
				else if (hitY == piece.y)
				{
					CurrentPieceDirection = (hitX < piece.x) ? EPuzzleDirection::Left : EPuzzleDirection::Right;
				}

				SelectionMode = PuzzleBlockSelectMode::Direction;
			}
		}
		else if (HitResult.Actor->GetClass() == ADefrostPuzzlePiece::StaticClass())
		{
			ADefrostPuzzlePiece* HitPiece = Cast<ADefrostPuzzlePiece>(HitResult.Actor.Get());
			HitPiece->bBlockInput = true;
		}
	}
#endif
}

void ADefrostPuzzlePawn::OnBlockMeshClicked(ADefrostPuzzleBlock* ClickedBlock, const std::pair<int32, int32>& position, const int index)
{
	if (!PuzzleBlockGrid->CanPlayerControllable()) return;

	switch (SelectionMode)
	{
	case ADefrostPuzzlePawn::PuzzleBlockSelectMode::None:
		break;
	case ADefrostPuzzlePawn::PuzzleBlockSelectMode::Piece:
		CurrentPieceIndex = ActivePieceIndex;
		break;
	case ADefrostPuzzlePawn::PuzzleBlockSelectMode::Direction:
	{
		if (CanMoveCurrentDirection == 0) return;
		PieceCommand command;
		command.PieceIndex = CurrentPieceIndex;
		command.PieceDirection = CurrentPieceDirection;
		command.BeforePosition = PuzzleBlockGrid->GetPuzzlePiecePositionByIndex(CurrentPieceIndex);
		command.BeforeScore = PuzzleBlockGrid->GetScore();
		PuzzleBlockGrid->MovePiece(CurrentPieceIndex, CurrentPieceDirection);
		command.AfterScore = PuzzleBlockGrid->GetScore();
		command.AfterPosition = PuzzleBlockGrid->GetPuzzlePiecePositionByIndex(CurrentPieceIndex);
		PieceCommands.Push(command);
		//PuzzleBlockGrid->UpdatePuzzlePiecesMesh();
		PuzzleBlockGrid->AddScore();
		UndoRedoCommands.Empty();
		break;
	}
	default:
		break;
	}

}

void ADefrostPuzzlePawn::OnPieceMeshClicked(ADefrostPuzzlePiece* ClickedPiece, const std::pair<int32, int32>& position, const int index)
{
	if (!PuzzleBlockGrid->CanPlayerControllable()) return;

	CurrentPieceIndex = index;
}
