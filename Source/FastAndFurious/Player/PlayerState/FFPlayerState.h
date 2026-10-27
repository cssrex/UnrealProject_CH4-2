
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FFPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnCoinsChanged);
DECLARE_MULTICAST_DELEGATE(FOnBonusMaxSpeedChanged);
DECLARE_MULTICAST_DELEGATE(FOnMaxSpeedPenaltyChanged);

UCLASS()
class FASTANDFURIOUS_API AFFPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(Server, Reliable)
	void ServerRPCAddCoins(const int32 Amount);

	int32 GetCoins() const { return Coins; }
	float GetBonusMaxSpeed() const { return BonusMaxSpeed; }
	float GetMaxSpeedPenalty() const { return MaxSpeedPenalty; }
	const FString& GetPlayerNameString() const { return PlayerNameString; }

	void AddCoins(int32 Amount);
	bool TryRemoveCoins(int32 Amount);
	void AddBonusMaxSpeed(float Amount);
	void SetPlayerNameString(FString& NewPlayerNameString);
	void AddMaxSpeedPenalty(float Amount);
	void RemoveMaxSpeedPenalty(float Amount);

	UFUNCTION(BlueprintCallable)
	FString GetPlayerStateInfo() const;

	UFUNCTION(BlueprintCallable)
	FString GetCoinsInfo() const;

	UFUNCTION(BlueprintCallable)
	FString GetBonusMaxSpeedInfo() const;

	UFUNCTION(BlueprintCallable)
	FString GetMaxSpeedPenaltyInfo() const;

public:
	FOnCoinsChanged OnCoinsChanged;
	FOnBonusMaxSpeedChanged OnBonusMaxSpeedChanged;
	FOnMaxSpeedPenaltyChanged OnMaxSpeedPenaltyChanged;

protected:
	UFUNCTION()
	void OnRep_Coins();

	UFUNCTION()
	void OnRep_BonusMaxSpeed();

	UFUNCTION()
	void OnRep_MaxSpeedPenalty();

protected:
	UPROPERTY(Replicated)
	FString PlayerNameString;

	UPROPERTY(ReplicatedUsing = OnRep_Coins)
	int32 Coins = 0;

	UPROPERTY(ReplicatedUsing = OnRep_BonusMaxSpeed)
	float BonusMaxSpeed = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_MaxSpeedPenalty)
	float MaxSpeedPenalty = 0.f;

};
