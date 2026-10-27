// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FFUI.generated.h"

/**
 *  Simple Vehicle HUD class
 *  Displays the current speed and gear.
 *  Widget setup is handled in a Blueprint subclass.
 */

class UFFChatUI;
class UTextBlock;
class AFFPlayerState;

UCLASS(abstract)
class FASTANDFURIOUS_API  UFFUI : public UUserWidget
{
	GENERATED_BODY()

protected:

	/** Controls the display of speed in Km/h or MPH */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Vehicle)
	bool bIsMPH = false;

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	void InitializePlayerState();

	void FocusChatInput();

	void UpdateSpeed(float NewSpeed);
	void UpdateGear(int32 NewGear);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = Vehicle)
	void OnSpeedUpdate(float NewSpeed);
	UFUNCTION(BlueprintImplementableEvent, Category = Vehicle)
	void OnGearUpdate(int32 NewGear);

	UFUNCTION()
	void OnCoinsTextCommitted();

	UFUNCTION()
	void OnMaxBonusSpeedTextCommitted();

	UFUNCTION()
	void OnMaxSpeedPenaltyTextCommitted();

	UFUNCTION()
	void OnNotificationTextCommitted(const FText& NewNotificationText);

#pragma region ChatUI
public:
	void OnNewChatMessage(const FString& Sender, const FString& Message);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFFChatUI> UI_Chat;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Coins;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_BonusMaxSpeed;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_MaxSpeedPenalty;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Notification;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_PlayerName;

	TObjectPtr<AFFPlayerState> CachingPlayerState;
#pragma endregion
};
