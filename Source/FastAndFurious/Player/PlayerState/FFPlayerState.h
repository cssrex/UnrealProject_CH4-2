
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FFPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnCoinsChanged);
DECLARE_MULTICAST_DELEGATE(FOnBonusMaxSpeedChanged);

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
	const FString& GetPlayerNameString() const { return PlayerNameString; }

	void AddCoins(int32 Amount);
	bool TryRemoveCoins(int32 Amount);
	void AddBonusMaxSpeed(float Amount);
	void SetPlayerNameString(FString& NewPlayerNameString);

	UFUNCTION(BlueprintCallable)
	FString GetPlayerStateInfo() const;

	UFUNCTION(BlueprintCallable)
	FString GetCoinsInfo() const;

	UFUNCTION(BlueprintCallable)
	FString GetBonusMaxSpeedInfo() const;

public:
	FOnCoinsChanged OnCoinsChanged;
	FOnBonusMaxSpeedChanged OnBonusMaxSpeedChanged;

protected:
	UFUNCTION()
	void OnRep_Coins();

	UFUNCTION()
	void OnRep_BonusMaxSpeed();

protected:
	UPROPERTY(Replicated)
	FString PlayerNameString;

	UPROPERTY(ReplicatedUsing = OnRep_Coins)
	int32 Coins = 0;

	UPROPERTY(ReplicatedUsing = OnRep_BonusMaxSpeed)
	float BonusMaxSpeed = 0.f;
};
