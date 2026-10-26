// Copyright Epic Games, Inc. All Rights Reserved.

#include "FFWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UFFWheelRear::UFFWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}