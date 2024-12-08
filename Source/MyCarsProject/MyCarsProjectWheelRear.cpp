// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyCarsProjectWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UMyCarsProjectWheelRear::UMyCarsProjectWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}