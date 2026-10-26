// Copyright Epic Games, Inc. All Rights Reserved.

#include "FFGameMode.h"
#include "Player/PlayerController/FFPlayerController.h"

AFFGameMode::AFFGameMode()
{
	PlayerControllerClass = AFFPlayerController::StaticClass();
}
