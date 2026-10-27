
#include "Player/PlayerState/FFPlayerState.h"
#include "FFPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/Pawn/FFPawn.h"

void AFFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, Coins, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, BonusMaxSpeed, COND_OwnerOnly);
	DOREPLIFETIME(ThisClass, PlayerNameString)
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

	OnCoinsChanged.Broadcast();
}

bool AFFPlayerState::TryRemoveCoins(int32 Amount)
{
	if (!HasAuthority() || Amount <= 0 || Coins < 5)
	{
		return false;
	}

	Coins -= Amount;

	OnCoinsChanged.Broadcast();

	return true;
}

void AFFPlayerState::AddBonusMaxSpeed(float Amount)
{
	if (!HasAuthority() || Amount <= 0)
	{
		return;
	}

	BonusMaxSpeed += Amount;

	OnBonusMaxSpeedChanged.Broadcast();
}

void AFFPlayerState::SetPlayerNameString(FString& NewPlayerNameString)
{
	if (!HasAuthority())
	{
		return;
	}

	PlayerNameString = NewPlayerNameString;
}

FString AFFPlayerState::GetPlayerStateInfo() const
{
	return FString::Printf(TEXT("Coins: %d, BonusMaxSpeed: %.2f"), Coins, BonusMaxSpeed);

}

FString AFFPlayerState::GetCoinsInfo() const
{
	return FString::Printf(TEXT("Coins: %d"), Coins);
}

FString AFFPlayerState::GetBonusMaxSpeedInfo() const
{
	return FString::Printf(TEXT("BonusMaxSpeed: % .2f"), BonusMaxSpeed);
}

void AFFPlayerState::OnRep_Coins()
{
	OnCoinsChanged.Broadcast();
}

void AFFPlayerState::OnRep_BonusMaxSpeed()
{
	OnBonusMaxSpeedChanged.Broadcast();
}

