
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FFGameState.generated.h"

class AFFPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotificationChanged, const FText&, NewText);
DECLARE_MULTICAST_DELEGATE(FOnGameStateChanged);

UENUM(BlueprintType)
enum class EGameState : uint8
{
	None,
	Waiting,
	Playing,
	Finished,
	End
};

UCLASS()
class FASTANDFURIOUS_API AFFGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void SetGameState(EGameState NewGameState);
	void SetRemainingTime(int32 NewRemainingTime);
	void SetWinners(TArray<AFFPlayerState*>& NewWinners);
	void SetNotificationText(const FText& NewText);

	int32 GetRemainingTime() const { return RemainingTime; }
	const FText& GetNotificationText() const { return NotificationText; }
	EGameState GetCurrentGameState() const { return CurrentGameState; }

protected:
	UFUNCTION()
	void OnRep_NotificationText();

	UFUNCTION()
	void OnRep_CurrentGameState();

public:
	FOnNotificationChanged OnNotificationChanged;
	FOnGameStateChanged OnGameStateChanged;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentGameState)
	EGameState CurrentGameState = EGameState::Waiting;

	UPROPERTY(Replicated)
	int32 RemainingTime = 0;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AFFPlayerState>> Winners;

	UPROPERTY(ReplicatedUsing = OnRep_NotificationText)
	FText NotificationText;
};
