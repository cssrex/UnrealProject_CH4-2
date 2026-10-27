
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FFGameStateBase.generated.h"

class ACXPlayerState;

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
class FASTANDFURIOUS_API AFFGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void SetGameState(EGameState NewGameState);
	void SetRemainingTime(int32 NewRemainingTime);
	void SetWinners(TArray<ACXPlayerState*>& NewWinners);

	int32 GetRemainingTime() const { return RemainingTime; }
protected:
	UPROPERTY(Replicated)
	EGameState CurrentGameState = EGameState::Waiting;

	UPROPERTY(Replicated)
	int32 RemainingTime = 0;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<ACXPlayerState>> Winners;
};
