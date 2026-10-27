
#include "FFGameStateBase.h"

#include "Net/UnrealNetwork.h"

void AFFGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentGameState);
	DOREPLIFETIME(ThisClass, RemainingTime);
	DOREPLIFETIME(ThisClass, Winners);
}

void AFFGameStateBase::SetGameState(EGameState NewGameState)
{
	if (!HasAuthority())
	{
		return;
	}

	CurrentGameState = NewGameState;
}

void AFFGameStateBase::SetRemainingTime(int32 NewRemainingTime)
{
	if (!HasAuthority())
	{
		return;
	}

	RemainingTime = FMath::Max(0, NewRemainingTime);
}

void AFFGameStateBase::SetWinners(TArray<ACXPlayerState*>& NewWinners)
{
	if (!HasAuthority())
	{
		return;
	}

	Winners = NewWinners;
}
