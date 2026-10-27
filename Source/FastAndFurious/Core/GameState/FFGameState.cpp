
#include "FFGameState.h"

#include "Net/UnrealNetwork.h"
#include "Player/PlayerState/FFPlayerState.h"

void AFFGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentGameState);
	DOREPLIFETIME(ThisClass, RemainingTime);
	DOREPLIFETIME(ThisClass, Winners);
	DOREPLIFETIME(ThisClass, NotificationText);
}

void AFFGameState::SetGameState(EGameState NewGameState)
{
	if (!HasAuthority())
	{
		return;
	}

	if (CurrentGameState == NewGameState)
	{
		return;
	}

	CurrentGameState = NewGameState;

	OnGameStateChanged.Broadcast();
}

void AFFGameState::SetRemainingTime(int32 NewRemainingTime)
{
	if (!HasAuthority())
	{
		return;
	}

	RemainingTime = FMath::Max(0, NewRemainingTime);
}

void AFFGameState::SetWinners(TArray<AFFPlayerState*>& NewWinners)
{
	if (!HasAuthority())
	{
		return;
	}

	Winners = NewWinners;
}

void AFFGameState::SetNotificationText(const FText& NewText)
{
	if (!HasAuthority())
	{
		return;
	}

	NotificationText = NewText;
}

void AFFGameState::OnRep_NotificationText()
{
	OnNotificationChanged.Broadcast(NotificationText);
}

void AFFGameState::OnRep_CurrentGameState()
{
	OnGameStateChanged.Broadcast();
}
