// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EPuzzleDirection : uint8
{
	Up,
	Left,
	Right,
	Down,
	Num  UMETA(Hidden)
};
