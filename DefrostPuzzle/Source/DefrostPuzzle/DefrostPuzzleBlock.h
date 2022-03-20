// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DefrostPuzzleBlock.generated.h"

UENUM(BlueprintType)
enum class EBlockType : uint8
{
	Rock,
	Frozen,
	HardFrozen,
	Melted,
	Num  UMETA(Hidden)
};

/** A block that can be clicked */
UCLASS(minimalapi)
class ADefrostPuzzleBlock : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** StaticMesh component for the clickable block */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BlockMesh;

public:
	ADefrostPuzzleBlock();

	/** Are we currently active? */
	bool bIsActive;

	/** Pointer to white material used on the focused block */
	UPROPERTY()
	class UMaterial* BaseMaterial;

	/** Pointer to material */
	UPROPERTY()
	class UMaterialInstance* RockMaterial;

	/** Pointer to material */
	UPROPERTY()
	class UMaterialInstance* FrozenMaterial;

	/** Pointer to material */
	UPROPERTY()
	class UMaterialInstance* HardFrozenMaterial;

	/** Pointer to material */
	UPROPERTY()
	class UMaterialInstance* MeltedMaterial;

	/** Pointer to blue material used on inactive blocks */
	UPROPERTY()
	class UMaterialInstance* BlueMaterial;

	/** Pointer to orange material used on active blocks */
	UPROPERTY()
	class UMaterialInstance* OrangeMaterial;

	/** Grid that owns us */
	UPROPERTY()
	class ADefrostPuzzleBlockGrid* OwningGrid;

	/** Handle the block being clicked */
	UFUNCTION()
	void BlockClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked);

	/** Handle the block being touched  */
	UFUNCTION()
	void OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	/** Set the block type */
	UFUNCTION()
	void SetBlockType(const EBlockType BlockType);

	/** Get the block type */
	UFUNCTION()
	EBlockType GetBlockType() const;

	void HandleClicked();

	void Highlight(bool bOn);

private:
	/** Pointer to white material used on the focused block */
	UPROPERTY()
	EBlockType BlockType;

public:
	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns BlockMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetBlockMesh() const { return BlockMesh; }
};



