// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyCarsProjectGameMode.h"
#include "MyCarsProjectPlayerController.h"

AMyCarsProjectGameMode::AMyCarsProjectGameMode()
{
	PlayerControllerClass = AMyCarsProjectPlayerController::StaticClass();
}
