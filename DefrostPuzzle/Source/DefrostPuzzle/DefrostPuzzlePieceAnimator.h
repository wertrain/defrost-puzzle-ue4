// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DefrostPuzzlePieceAnimator.generated.h"

// This class does not need to be modified.
UINTERFACE(minimalapi, Blueprintable)
class UDefrostPuzzlePieceAnimator : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEFROSTPUZZLE_API IDefrostPuzzlePieceAnimator
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="DefrostPuzzle|Animation")
	bool IsSlideAnimation() const;
	bool IsSlideAnimation_Implementation() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DefrostPuzzle|Animation")
	bool IsChildIdle() const;
	bool IsChildIdle_Implementation() const;

	void SetSliding(const bool Sliding);
	void SetChildIdle(const bool ChildIdle);

private:
	bool IsSliding{ false };
	bool IsChildIdling { false };
};
