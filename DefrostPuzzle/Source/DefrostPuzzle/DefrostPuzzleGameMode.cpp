// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefrostPuzzleGameMode.h"
#include "DefrostPuzzlePlayerController.h"
#include "DefrostPuzzlePawn.h"

ADefrostPuzzleGameMode::ADefrostPuzzleGameMode()
{
	// no pawn by default
	DefaultPawnClass = ADefrostPuzzlePawn::StaticClass();
	// use our own player controller class
	PlayerControllerClass = ADefrostPuzzlePlayerController::StaticClass();
}
