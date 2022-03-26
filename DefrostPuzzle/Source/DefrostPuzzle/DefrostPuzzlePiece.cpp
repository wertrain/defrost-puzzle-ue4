// Fill out your copyright notice in the Description page of Project Settings.


#include "DefrostPuzzlePiece.h"

// Sets default values
ADefrostPuzzlePiece::ADefrostPuzzlePiece()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<USkeletalMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UPhysicsAsset> PhysicsAsset;
		ConstructorHelpers::FObjectFinderOptional<UAnimBlueprint> AnimBlueprint;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/SK_Penguin.SK_Penguin"))
			, PhysicsAsset(TEXT("/Game/Puzzle/SK_Penguin_Physics.SK_Penguin_Physics"))
			, AnimBlueprint(TEXT("/Game/Puzzle/ABP_Penguin.ABP_Penguin"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component
	const float scale = 0.1f;
	PieceMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PenguinMesh0"));
	PieceMesh->SetSkeletalMesh(ConstructorStatics.PlaneMesh.Get());
	PieceMesh->SetRelativeScale3D(FVector(1.f * scale, 1.f * scale, 1.f * scale));
	PieceMesh->SetRelativeLocation(FVector(0.f, 0.f, 1.f));
	PieceMesh->SetPhysicsAsset(ConstructorStatics.PhysicsAsset.Get());
	//PieceMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	//PieceMesh->SetupAttachment(DummyRoot);
	//PieceMesh->OnClicked.AddDynamic(this, &ADefrostPuzzleBlock::BlockClicked);
	//PieceMesh->OnInputTouchBegin.AddDynamic(this, &ADefrostPuzzleBlock::OnFingerPressedBlock);
	PieceMesh->SetAnimInstanceClass(ConstructorStatics.AnimBlueprint.Get()->GeneratedClass);
}

// Called when the game starts or when spawned
void ADefrostPuzzlePiece::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADefrostPuzzlePiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADefrostPuzzlePiece::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

