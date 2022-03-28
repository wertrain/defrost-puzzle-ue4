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

protected:
	// Begin AActor interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End AActor interface

public:

	/** Handle the block being clicked */
	void AddScore();
	void ResetScore();

	// 指定された位置（x, y）のブロックを取得
	class ADefrostPuzzleBlock* GetPuzzleBlock(const int x, const int y);
	// 指定されたインデックスのブロックを取得
	class ADefrostPuzzleBlock* GetPuzzleBlock(const int index);
	int32 GetPuzzleBlockIndex(const class ADefrostPuzzleBlock* Block) const;
	// 指定されたブロックの位置（x, y）を取得
	std::pair<int32, int32> GetPuzzleBlockPosition(const class ADefrostPuzzleBlock* Block) const;
	// 指定されたピースの位置（x, y）を取得
	std::pair<int32, int32> GetPuzzlePiecePositionByIndex(const int PieceIndex) const;
	// 指定されたピースのインデックスを取得
	int32 GetPuzzlePieceIndex(const class ADefrostPuzzlePiece* Piece) const;
	// すべてのピースを取得
	const TArray<game::Field::Position>& GetPieces() const;
	// 指定されたインデックスのピースから、指定されたブロックをハイライトを設定
	void SetHighlightBlock(const int PieceIndex);
	// 指定されたインデックスのピースから、指定された方向のブロックにハイライトを設定
	void SetHighlightDirection(const int PieceIndex, const EPuzzleDirection Direction);
	// 指定されたインデックスのピースを、指定された方向に向く
	void SetPieceDirection(const int PieceIndex, const EPuzzleDirection Direction);
	// 指定されたインデックスのピースを、指定された方向に移動
	bool MovePiece(const int PieceIndex, const EPuzzleDirection Direction);
	// すべてのピースを初期位置に戻す
	void ResetPieces();
	// すべてのピースの位置を反映
	void UpdatePuzzlePiecesMesh();
	// 指定されたインデックスがゴールかを判定
	bool IsGoal(const int Index) const;
	// 指定されたブロックの上にピースが乗っているかを判定
	int IsOnPiece(const class ADefrostPuzzleBlock* Block) const;
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

	class PuzzleBlockGridSequenceBase
	{
	public:
		PuzzleBlockGridSequenceBase(ADefrostPuzzleBlockGrid* Owner) : Owner(Owner) {}
		virtual ~PuzzleBlockGridSequenceBase() {}
		virtual void Update(float DeltaSeconds) = 0;
		virtual bool CanPlayerControl() const { return false; }
	protected:
		ADefrostPuzzleBlockGrid* Owner;
	};

	class SequenceNop : public PuzzleBlockGridSequenceBase
	{
	public:
		SequenceNop(ADefrostPuzzleBlockGrid* Owner) : PuzzleBlockGridSequenceBase(Owner) {}
		void Update(float DeltaSeconds) override {}
		bool CanPlayerControl() const override { return true; }
	};

	class SequenceMovePiece : public PuzzleBlockGridSequenceBase
	{
	public:
		SequenceMovePiece(ADefrostPuzzleBlockGrid* Owner);
		void Update(float DeltaSeconds) override;
		void SetTarget(class ADefrostPuzzlePiece* Piece, const game::Field::Position& Start, const game::Field::Position& Goal);
		bool IsMoveEnd() const;

	private:
		class ADefrostPuzzlePiece* TargetPiece;
		FVector StartPieceLocation;
		FVector EndPieceLocation;
		float CurrentTime;
	};

	template <class SEQUENCE>
	SEQUENCE* NextSequence()
	{
		Sequence = std::make_unique<SEQUENCE>(this);
		return static_cast<SEQUENCE*>(Sequence.get());
	}

private:
	std::unique_ptr<game::Field> Field;
	TArray<game::Field::Position> PiecePositions;
	TArray<game::Field::Position> DefaultPiecePositions;
	TArray<class ADefrostPuzzleBlock*> PuzzleBlocks;
	TArray<class ADefrostPuzzlePiece*> PuzzlePieces;
	class ADefrostPuzzlePiece* PuzzleGoalPiece;
	TArray<class IDefrostPuzzleBlockGridListener*> Listeners;
	std::unique_ptr<PuzzleBlockGridSequenceBase> Sequence;
};
