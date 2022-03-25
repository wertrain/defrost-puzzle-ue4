// Fill out your copyright notice in the Description page of Project Settings.


#include "DefrostPuzzlePieceAnimator.h"

// Add default functionality here for any IDefrostPuzzlePieceAnimator functions that are not pure virtual.

bool IDefrostPuzzlePieceAnimator::IsSlideAnimation_Implementation() const
{
    return IsSliding;
}

void IDefrostPuzzlePieceAnimator::SetSliding(const bool Sliding)
{
    IsSliding = Sliding;
}