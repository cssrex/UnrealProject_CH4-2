
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FFPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnStateChanged);

UCLASS()
class FASTANDFURIOUS_API AFFPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(Server, Reliable)
	void ServerRPCAddCoins(const int32 Amount);

	int32 GetCoin() const { return Coins; }
	float GetBonusMaxSpeed() const { return BonusMaxSpeed; }

	void AddCoins(int32 Amount);
	bool TryRemoveCoins(int32 Amount);
	void AddBonusMaxSpeed(float Amount);

	UFUNCTION(BlueprintCallable)
	FString GetPlayerStateInfo() const;

public:
	FOnStateChanged OnStateChanged;

protected:
	UFUNCTION()
	void OnRep_State();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_State)
	int32 Coins = 0;

	UPROPERTY(ReplicatedUsing = OnRep_State)
	float BonusMaxSpeed = 0.f;
};
