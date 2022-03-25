// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <memory>
#include "Game/Field.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DefrostPuzzleTypes.h"
#include "DefrostPuzzleBlockGrid.generated.h"

/** IDefrostPuzzleBlockGridListener */
class IDefrostPuzzleBlockGridListener
{
public:
	virtual void OnBlockMeshClicked(class ADefrostPuzzleBlock* ClickedBlock, const std::pair<int32, int32>& position, const int index) = 0;
	virtual void OnPieceMeshClicked(class ADefrostPuzzlePiece* ClickedPiece, const std::pair<int32, int32>& position, const int index) = 0;
};

/** Class used to spawn blocks and manage score */
UCLASS(minimalapi)
class ADefrostPuzzleBlockGrid : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** Text component for the score */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* ScoreText;

public:
	ADefrostPuzzleBlockGrid();

	/** How many blocks have been clicked */
	int32 Score;

	/** Width of blocks along each side of grid */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	int32 Width;

	/** Height of blocks along each side of grid */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	int32 Height;

	/** Spacing of blocks */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	float BlockSpacing;

public:
	UFUNCTION(BlueprintCallable, Category=Grid)
	void UpdatePuzzlePiecesMesh();

protected:
	// Begin AActor interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End AActor interface

public:

	/** Handle the block being clicked */
	void AddScore();

	// 指定された位置（x, y）のブロックを取得
	class ADefrostPuzzleBlock* GetPuzzleBlock(const int x, const int y);
	// 指定されたインデックスのブロックを取得
	class ADefrostPuzzleBlock* GetPuzzleBlock(const int index);
	int32 GetPuzzleBlockIndex(const class ADefrostPuzzleBlock* Block) const;
	// 指定されたブロックの位置（x, y）を取得
	std::pair<int32, int32> GetPuzzleBlockPosition(const class ADefrostPuzzleBlock* Block) const;
	// 指定されたピースのインデックスを取得
	int32 GetPuzzlePieceIndex(const class ADefrostPuzzlePiece* Piece) const;
	// すべてのピースを取得
	const std::vector<game::Field::Position>& GetPieces() const;
	// 指定されたインデックスのピースから、指定された方向のブロックにハイライトを設定
	void SetHighlightDirection(const int PieceIndex, const EPuzzleDirection Direction);
	// 指定されたインデックスのピースを、指定された方向に移動
	bool MovePiece(const int PieceIndex, const EPuzzleDirection Direction);
	// すべてのピースを初期位置に戻す
	void ResetPiece();
	// 指定されたインデックスがゴールかを判定
	bool IsGoal(const int Index) const;
	// リスナーを追加
	void AddListener(IDefrostPuzzleBlockGridListener* Listener);

	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns ScoreText subobject **/
	FORCEINLINE class UTextRenderComponent* GetScoreText() const { return ScoreText; }

private:
	// ピースが配置されている位置から、指定方向のブロックを取得する、最終位置がゴールに到達しているかも判断できる（盛りだくさん）
	std::pair<int32, int32> GetPuzzleBlockLine(const int PieceIndex, const EPuzzleDirection Direction, std::vector<class ADefrostPuzzleBlock*>& OutList, bool &OutIsGoal);
	
	UFUNCTION()
	void BlockMeshClicked(UPrimitiveComponent* ClickedComponent, FKey ButtonClicked);
	UFUNCTION()
	void PieceMeshClicked(UPrimitiveComponent* ClickedComponent, FKey ButtonClicked);
	UFUNCTION()
	void OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	enum class ESequence : uint8
	{
		Nop,
		MovePiece
	};

private:
	std::unique_ptr<game::Field> Field;
	std::vector<game::Field::Position> PiecePositions;
	std::vector<game::Field::Position> DefaultPiecePositions;
	TArray<class ADefrostPuzzleBlock*> PuzzleBlocks;
	TArray<class ADefrostPuzzlePiece*> PuzzlePieces;
	TArray<class IDefrostPuzzleBlockGridListener*> Listeners;
	ESequence Sequence;
};
