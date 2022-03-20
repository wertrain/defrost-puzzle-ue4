// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DefrostPuzzlePiece.generated.h"

UCLASS()
class DEFROSTPUZZLE_API ADefrostPuzzlePiece : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADefrostPuzzlePiece();

	// StaticMesh component 
	UPROPERTY(Category = Piece, VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UStaticMeshComponent* PieceMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// Returns BlockMesh subobject
	FORCEINLINE class UStaticMeshComponent* GetPieceMesh() const { return PieceMesh; }
};