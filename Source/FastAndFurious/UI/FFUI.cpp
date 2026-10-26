
#include "FFUI.h"

void UFFUI::UpdateSpeed(float NewSpeed)
{
	float FormattedSpeed = FMath::Abs(NewSpeed) * (bIsMPH ? 0.022f : 0.036f);

	OnSpeedUpdate(FormattedSpeed);
}

void UFFUI::UpdateGear(int32 NewGear)
{
	OnGearUpdate(NewGear);
}