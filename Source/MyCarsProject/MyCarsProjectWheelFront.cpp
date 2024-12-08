// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyCarsProjectWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UMyCarsProjectWheelFront::UMyCarsProjectWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}