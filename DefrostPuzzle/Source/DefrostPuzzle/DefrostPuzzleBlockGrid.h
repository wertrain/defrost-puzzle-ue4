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
	UFUNCTION(BlueprintImplementableEvent, Category = Grid)
	void OnGameFinished();

	UFUNCTION(BlueprintCallable, Category=Grid)
	FText GetFieldCode() const;

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
	void SetScore(const int NewScore);
	int32 GetScore() const;

	// �w�肳�ꂽ�ʒu�ix, y�j�̃u���b�N���擾
	class ADefrostPuzzleBlock* GetPuzzleBlock(const int x, const int y);
	// �w�肳�ꂽ�C���f�b�N�X�̃u���b�N���擾
	class ADefrostPuzzleBlock* GetPuzzleBlock(const int index);
	int32 GetPuzzleBlockIndex(const class ADefrostPuzzleBlock* Block) const;
	// �w�肳�ꂽ�u���b�N�̈ʒu�ix, y�j���擾
	std::pair<int32, int32> GetPuzzleBlockPosition(const class ADefrostPuzzleBlock* Block) const;
	// �w�肳�ꂽ�s�[�X�̈ʒu�ix, y�j���擾
	std::pair<int32, int32> GetPuzzlePiecePositionByIndex(const int PieceIndex) const;
	// �w�肳�ꂽ�s�[�X�̃C���f�b�N�X���擾
	int32 GetPuzzlePieceIndex(const class ADefrostPuzzlePiece* Piece) const;
	// ���ׂẴs�[�X���擾
	const TArray<game::Field::Position>& GetPieces() const;
	// �w�肳�ꂽ�C���f�b�N�X�̃s�[�X����A�w�肳�ꂽ�u���b�N���n�C���C�g��ݒ�
	void SetHighlightBlock(const int PieceIndex);
	// �w�肳�ꂽ�C���f�b�N�X�̃s�[�X����A�w�肳�ꂽ�����̃u���b�N�Ƀn�C���C�g��ݒ�
	int SetHighlightDirection(const int PieceIndex, const EPuzzleDirection Direction);
	// �n�C���C�g���ꂽ�u���b�N�����ׂă��Z�b�g
	void ResetHighlightAll();
	// �w�肳�ꂽ�C���f�b�N�X�̃s�[�X���A�w�肳�ꂽ�����Ɍ���
	void SetPieceDirection(const int PieceIndex, const EPuzzleDirection Direction);
	// �w�肳�ꂽ�s�[�X�̈ʒu�ix, y�j��ݒ�
	void SetPiecePosition(const int PieceIndex, const std::pair<int32, int32> Position);
	// �w�肳�ꂽ�C���f�b�N�X�̃s�[�X���A�w�肳�ꂽ�����Ɉړ�
	bool MovePiece(const int PieceIndex, const EPuzzleDirection Direction);
	// ���ׂẴs�[�X�������ʒu�ɖ߂�
	void ResetPieces();
	// ���ׂẴs�[�X�̈ʒu�𔽉f
	void UpdatePuzzlePiecesMesh();
	// �w�肳�ꂽ�C���f�b�N�X���S�[�����𔻒�
	bool IsGoal(const int Index) const;
	// �w�肳�ꂽ�C�ʒu�̎��͂ɃS�[�������邩�𔻒�
	bool CheckGoal(const game::Field::Position& Position) const;
	// �w�肳�ꂽ�u���b�N�̏�Ƀs�[�X������Ă��邩�𔻒�
	int IsOnPiece(const class ADefrostPuzzleBlock* Block) const;
	// ���݂�����\�����擾
	bool CanPlayerControllable() const;
	// ���X�i�[��ǉ�
	void AddListener(IDefrostPuzzleBlockGridListener* Listener);

	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns ScoreText subobject **/
	FORCEINLINE class UTextRenderComponent* GetScoreText() const { return ScoreText; }

private:
	// �s�[�X���z�u����Ă���ʒu����A�w������̃u���b�N���擾����A�ŏI�ʒu���S�[���ɓ��B���Ă��邩�����f�ł���i���肾������j
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
	private:
		PuzzleBlockGridSequenceBase() {}
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
		enum class InSequenceId : uint8
		{
			InAnim,
			Sliding,
			OutAnim
		};
		void Update_InAnim(float DeltaSeconds);
		void Update_Sliding(float DeltaSeconds);
		void Update_OutAnim(float DeltaSeconds);

	private:
		class ADefrostPuzzlePiece* TargetPiece;
		FVector StartPieceLocation;
		FVector EndPieceLocation;
		float CurrentTime;
		float TargetTime;
		InSequenceId InSequence;
		float AnimTime;
		bool IsGoal;
	};

	class SequenceFinished : public PuzzleBlockGridSequenceBase
	{
	public:
		SequenceFinished(ADefrostPuzzleBlockGrid* Owner) : PuzzleBlockGridSequenceBase(Owner) {}
		void Update(float DeltaSeconds) override;

	private:
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
