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
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> SubMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> ChildMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/SK_Penguin.SK_Penguin"))
			, PhysicsAsset(TEXT("/Game/Puzzle/SK_Penguin_Physics.SK_Penguin_Physics"))
			, AnimBlueprint(TEXT("/Game/Puzzle/ABP_Penguin.ABP_Penguin"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/M_Penguin.M_Penguin"))
			, SubMaterial(TEXT("/Game/Puzzle/Meshes/M_Penguin_Sub.M_Penguin_Sub"))
			, ChildMaterial(TEXT("/Game/Puzzle/Meshes/M_Penguin_Child.M_Penguin_Child"))
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

	BaseMaterial = ConstructorStatics.BaseMaterial.Get();
	SubMaterial = ConstructorStatics.SubMaterial.Get();
	ChildMaterial = ConstructorStatics.ChildMaterial.Get();
}

void ADefrostPuzzlePiece::SetPieceType(const EPieceType PieceType)
{
	UMaterial* TypeToMaterial[static_cast<uint8>(EPieceType::Num)] =
	{
		BaseMaterial,
		SubMaterial,
		ChildMaterial
	};

	// Change material
	PieceMesh->SetMaterial(0, TypeToMaterial[static_cast<uint8>(PieceType)]);
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

