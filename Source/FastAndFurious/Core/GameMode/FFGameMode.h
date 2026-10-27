
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FFGameMode.generated.h"

UCLASS(MinimalAPI)
class AFFGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFFGameMode();

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:
	// 게임 시작
	void StartWaitingCountdown();
	void HandleWaitingTimerTick();

	// 게임 진행
	void StartPlayingState();
	void HandlePlayingTimerTick();

	// 게임 끝
	void FinishGame();

private:
	bool IsPlayerStartAvailable(const APlayerStart* PlayerStart) const;

private:
	UPROPERTY(EditDefaultsOnly)
	FVector VehicleSpawnCheckHalfExtent = FVector(250.0, 120.0, 80.0);

	UPROPERTY(EditDefaultsOnly)
	FVector VehicleSpawnCheckOffset = FVector(0.0, 0.0, 80.0);

	bool bWaitingCountdownStarted = false;

	FTimerHandle GameTimerHandle;
};



