// Copyright Epic Games, Inc. All Rights Reserved.

#include "FFWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UFFWheelFront::UFFWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}