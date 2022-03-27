// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DefrostPuzzleTypes.h"
#include "DefrostPuzzleBlockGrid.h"
#include "DefrostPuzzlePawn.generated.h"

UCLASS(config=Game)
class ADefrostPuzzlePawn : public APawn, public IDefrostPuzzleBlockGridListener
{
	GENERATED_UCLASS_BODY()

public:

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	UFUNCTION(BlueprintCallable, Category = Grid)
	void ResetAllPieces();

protected:
	void OnResetVR();
	void TriggerClick();
	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

	void OnBlockMeshClicked(class ADefrostPuzzleBlock* ClickedBlock, const std::pair<int32, int32>& position, const int index) override;
	void OnPieceMeshClicked(class ADefrostPuzzlePiece* ClickedPiece, const std::pair<int32, int32>& position, const int index) override;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class ADefrostPuzzleBlock* CurrentBlockFocus;

private:
	enum class PuzzleBlockSelectMode : uint8
	{
		None,
		Piece,
		Direction
	};

	struct PieceCommand
	{
		int32 PieceIndex;
		EPuzzleDirection PieceDirection;
	};

private:
	class ADefrostPuzzleBlockGrid* PuzzleBlockGrid;
	int32 ActivePieceIndex;
	int32 CurrentPieceIndex;
	EPuzzleDirection CurrentPieceDirection;
	PuzzleBlockSelectMode SelectionMode;
	TArray<PieceCommand> PieceCommands;
};
