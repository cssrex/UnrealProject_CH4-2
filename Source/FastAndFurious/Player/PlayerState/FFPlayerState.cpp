
#include "Player/PlayerState/FFPlayerState.h"
#include "FFPlayerState.h"
#include "Net/UnrealNetwork.h"

void AFFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, Coins, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, BonusMaxSpeed, COND_OwnerOnly);
}

void AFFPlayerState::ServerRPCAddCoins_Implementation(const int32 Amount)
{
	AddCoins(Amount);
}


void AFFPlayerState::AddCoins(int32 Amount)
{
	if (!HasAuthority() || Amount <= 0)
	{
		return;
	}

	FString PlayerName = GetPlayerName();
	UE_LOG(LogTemp, Warning, TEXT("%s - AddCoins: %d(+%d)"), *PlayerName, Coins, Amount);
	Coins += Amount;
	OnStateChanged.Broadcast();
}

bool AFFPlayerState::TryRemoveCoins(int32 Amount)
{
	if (!HasAuthority() || Amount <= 0 || Coins < 5)
	{
		return false;
	}

	Coins -= Amount;

	OnStateChanged.Broadcast();

	return true;
}

void AFFPlayerState::AddBonusMaxSpeed(float Amount)
{
	if (!HasAuthority() || Amount <= 0)
	{
		return;
	}

	BonusMaxSpeed += Amount;

	OnStateChanged.Broadcast();
}

FString AFFPlayerState::GetPlayerStateInfo() const
{
	return FString::Printf(TEXT("Coins: %d, BonusMaxSpeed: %.2f"), Coins, BonusMaxSpeed);

}

void AFFPlayerState::OnRep_State()
{
	OnStateChanged.Broadcast();
}

