
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FFGameMode.generated.h"

class AFFPlayerController;

UCLASS(MinimalAPI)
class AFFGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFFGameMode();

protected:
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	virtual void BeginPlay() override;
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

	// 현재 인원에 따라 대기 공지와 카운트다운 처리
	void UpdateWaitingState();

	// 경기 종료 후 클라이언트를 타이틀로 이동
	void ReturnPlayersToTitle();

	// 클라이언트 연결 종료를 확인한 뒤 서버 초기화
	void TryResetServer();

private:
	bool IsPlayerStartAvailable(const APlayerStart* PlayerStart) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AFFPlayerController>> AlivePlayerControllers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AFFPlayerController>> DeadPlayerControllers;

private:
	UPROPERTY(EditDefaultsOnly)
	FVector VehicleSpawnCheckHalfExtent = FVector(250.0, 120.0, 80.0);

	UPROPERTY(EditDefaultsOnly)
	FVector VehicleSpawnCheckOffset = FVector(0.0, 0.0, 80.0);

	bool bWaitingCountdownStarted = false;

	FTimerHandle GameTimerHandle;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Game|Match")
	int32 MinimumPlayerCount = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Game|Match")
	int32 WaitingDuration = 15;

	UPROPERTY(EditDefaultsOnly, Category = "Game|Match")
	int32 PlayingDuration = 30;

	UPROPERTY(EditDefaultsOnly, Category = "Game|Match")
	float ReturnToTitleDelay = 5.0f;

	// 실제 타이틀 레벨 경로로 설정
	UPROPERTY(EditDefaultsOnly, Category = "Game|Match")
	FString TitleMapPath = TEXT("/Game/Blueprints/Levels/L_Title");

	FTimerHandle ResetTimerHandle;

	bool bWaitingForPlayersToLeave = false;
};



