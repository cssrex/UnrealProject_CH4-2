
#include "Item/FFSpeedBuffItem.h"

#include "Player/PlayerState/FFPlayerState.h"

AFFSpeedBuffItem::AFFSpeedBuffItem()
{
}

void AFFSpeedBuffItem::PickupAction(APawn* VehiclePawn)
{
	AFFPlayerState* PlayerState = Cast<AFFPlayerState>(VehiclePawn->GetPlayerState());
	if (IsValid(PlayerState))
	{
		PlayerState->AddBonusMaxSpeed(20);
	}
}