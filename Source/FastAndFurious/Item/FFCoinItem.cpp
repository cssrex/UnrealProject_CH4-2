
#include "FFCoinItem.h"

#include "Player/PlayerState/FFPlayerState.h"

AFFCoinItem::AFFCoinItem()
{
}

void AFFCoinItem::PickupAction(APawn* VehiclePawn)
{
	AFFPlayerState* PlayerState = Cast<AFFPlayerState>(VehiclePawn->GetPlayerState());
	if (IsValid(PlayerState))
	{
		PlayerState->AddCoins(10);
	}
}

