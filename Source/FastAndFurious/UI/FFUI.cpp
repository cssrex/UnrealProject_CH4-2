
#include "FFUI.h"
#include "UI/FFChatUI.h"
#include "Components/TextBlock.h"
#include "Player/PlayerState/FFPlayerState.h"
#include "Core/GameState/FFGameState.h"

void UFFUI::NativeConstruct()
{
	Super::NativeConstruct();

	InitializePlayerState();
}

void UFFUI::NativeDestruct()
{
	if (IsValid(CachingPlayerState))
	{
		CachingPlayerState->OnCoinsChanged.RemoveAll(this);
		CachingPlayerState->OnBonusMaxSpeedChanged.RemoveAll(this);
	}

	if (UWorld* World = GetWorld())
	{
		AFFGameState* FFGameState = World->GetGameState<AFFGameState>();

		if (IsValid(FFGameState))
		{
			if (FFGameState->OnNotificationChanged.IsAlreadyBound(this, &UFFUI::OnNotificationTextCommitted) == true)
			{
				FFGameState->OnNotificationChanged.RemoveDynamic(this, &UFFUI::OnNotificationTextCommitted);
			}
		}
	}

	Super::NativeDestruct();
}

void UFFUI::InitializePlayerState()
{
	APlayerController* PC = GetOwningPlayer();
	if (!IsValid(PC))
	{
		return;
	}

	AFFPlayerState* NewPlayerState = PC->GetPlayerState<AFFPlayerState>();
	if (!IsValid(NewPlayerState))
	{
		return;
	}

	if (IsValid(CachingPlayerState))
	{
		CachingPlayerState->OnCoinsChanged.RemoveAll(this);
		CachingPlayerState->OnBonusMaxSpeedChanged.RemoveAll(this);
	}

	CachingPlayerState = NewPlayerState;
	CachingPlayerState->OnCoinsChanged.AddUObject(this, &UFFUI::OnCoinsTextCommitted);
	CachingPlayerState->OnBonusMaxSpeedChanged.AddUObject(this, &UFFUI::OnMaxBonusSpeedTextCommitted);
	CachingPlayerState->OnMaxSpeedPenaltyChanged.AddUObject(this, &UFFUI::OnMaxSpeedPenaltyTextCommitted);

	OnCoinsTextCommitted();
	OnMaxBonusSpeedTextCommitted();
	OnMaxSpeedPenaltyTextCommitted();

	if (UWorld* World = GetWorld())
	{
		AFFGameState* FFGameState = World->GetGameState<AFFGameState>();

		if (IsValid(FFGameState))
		{
			if (FFGameState->OnNotificationChanged.IsAlreadyBound(this, &UFFUI::OnNotificationTextCommitted) == false)
			{
				FFGameState->OnNotificationChanged.AddDynamic(this, &UFFUI::OnNotificationTextCommitted);
			}

			OnNotificationTextCommitted(FFGameState->GetNotificationText());
		}
	}

	Text_PlayerName->SetText(FText::FromString(FString::Printf(TEXT("YOU : %s"), *CachingPlayerState->GetPlayerNameString())));
}

void UFFUI::FocusChatInput()
{
	if (IsValid(UI_Chat))
	{
		UI_Chat->FocusChatInput();
	}
}

void UFFUI::UpdateSpeed(float NewSpeed)
{
	float FormattedSpeed = FMath::Abs(NewSpeed) * (bIsMPH ? 0.022f : 0.036f);

	OnSpeedUpdate(FormattedSpeed);
}

void UFFUI::UpdateGear(int32 NewGear)
{
	OnGearUpdate(NewGear);
}

void UFFUI::OnCoinsTextCommitted()
{
	if (IsValid(CachingPlayerState) && IsValid(Text_Coins))
	{
		FString Info = CachingPlayerState->GetCoinsInfo();
		Text_Coins->SetText(FText::FromString(Info));
	}
}

void UFFUI::OnMaxBonusSpeedTextCommitted()
{
	if (IsValid(CachingPlayerState) && IsValid(Text_BonusMaxSpeed))
	{
		FString Info = CachingPlayerState->GetBonusMaxSpeedInfo();
		Text_BonusMaxSpeed->SetText(FText::FromString(Info));
	}
}

void UFFUI::OnMaxSpeedPenaltyTextCommitted()
{
	if (IsValid(CachingPlayerState) && IsValid(Text_MaxSpeedPenalty))
	{
		FString Info = CachingPlayerState->GetMaxSpeedPenaltyInfo();
		Text_MaxSpeedPenalty->SetText(FText::FromString(Info));
	}
}

void UFFUI::OnNotificationTextCommitted(const FText& NewNotificationText)
{
	if (IsValid(Text_Notification))
	{
		Text_Notification->SetText(NewNotificationText);
	}
}

void UFFUI::OnNewChatMessage(const FString& Sender, const FString& Message)
{
	UI_Chat->OnNewChatMessage(Sender, Message);
}

