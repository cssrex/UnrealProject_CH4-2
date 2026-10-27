
#include "FFUI.h"
#include "UI/FFChatUI.h"
#include "Components/TextBlock.h"
#include "Player/PlayerState/FFPlayerState.h"

void UFFUI::NativeConstruct()
{
	Super::NativeConstruct();

	InitializePlayerState();
}

void UFFUI::NativeDestruct()
{
	if (IsValid(CachingPlayerState))
	{
		CachingPlayerState->OnStateChanged.RemoveAll(this);
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
		CachingPlayerState->OnStateChanged.RemoveAll(this);
	}

	CachingPlayerState = NewPlayerState;
	CachingPlayerState->OnStateChanged.AddUObject(this, &UFFUI::OnStateTextCommitted);

	OnStateTextCommitted();
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

void UFFUI::OnStateTextCommitted()
{
	if (IsValid(CachingPlayerState) && IsValid(StateText))
	{
		FString Info = CachingPlayerState->GetPlayerStateInfo();
		StateText->SetText(FText::FromString(Info));
	}
}

void UFFUI::OnNewChatMessage(const FString& Sender, const FString& Message)
{
	UI_Chat->OnNewChatMessage(Sender, Message);
}

