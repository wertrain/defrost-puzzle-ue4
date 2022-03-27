// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DefrostPuzzlePieceAnimator.h"
#include "DefrostPuzzlePiece.generated.h"

UENUM(BlueprintType)
enum class EPieceType : uint8
{
	Main,
	Sub,
	Goal,
	Num  UMETA(Hidden)
};

UCLASS()
class DEFROSTPUZZLE_API ADefrostPuzzlePiece : public APawn, public IDefrostPuzzlePieceAnimator
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADefrostPuzzlePiece();

	// StaticMesh component 
	UPROPERTY(Category=Piece, VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* PieceMesh;

	/** Pointer to material */
	UPROPERTY()
	class UMaterial* BaseMaterial;

	/** Pointer to material */
	UPROPERTY()
	class UMaterial* SubMaterial;

	/** Pointer to material */
	UPROPERTY()
	class UMaterial* ChildMaterial;

	/** Set the block type */
	UFUNCTION()
	void SetPieceType(const EPieceType PieceType);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FORCEINLINE class USkeletalMeshComponent* GetPieceMesh() const { return PieceMesh; }


};
