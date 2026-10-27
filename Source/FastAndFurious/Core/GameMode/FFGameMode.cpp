
#include "FFGameMode.h"
#include "Player/PlayerController/FFPlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Core/Subsystem/FFChatSubsystem.h"
#include "Player/PlayerState/FFPlayerState.h"
#include "Core/GameState/FFGameState.h"

AFFGameMode::AFFGameMode()
{
	PlayerControllerClass = AFFPlayerController::StaticClass();
}

void AFFGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (IsValid(NewPlayer) == false)
	{
		return;
	}

	AFFPlayerController* FFPlayerController = Cast<AFFPlayerController>(NewPlayer);
	if (IsValid(FFPlayerController))
	{
		AlivePlayerControllers.Add(FFPlayerController);

		AFFPlayerState* FFPlayerState = FFPlayerController->GetPlayerState<AFFPlayerState>();
		if (IsValid(FFPlayerState))
		{
			FString PlayerName = FString::Printf(TEXT("Player%d"), AlivePlayerControllers.Num() + DeadPlayerControllers.Num());
			FFPlayerState->SetPlayerNameString(PlayerName);
		}

		if (!bWaitingCountdownStarted && AlivePlayerControllers.Num() >= 2)
		{
			StartWaitingCountdown();
		}

		if (UFFChatSubsystem* ChatSubsystem = GetWorld()->GetSubsystem<UFFChatSubsystem>())
		{
			const TArray<FChatData> ChatLog = ChatSubsystem->GetChatLog();

			if (!ChatLog.IsEmpty())
			{
				FFPlayerController->ClientRPCChatLog(ChatLog);
			}
		}
	}
}

void AFFGameMode::Logout(AController* Exiting)
{
	AFFPlayerController* ExitingPlayerController = Cast<AFFPlayerController>(Exiting);
	if (IsValid(ExitingPlayerController) == true && AlivePlayerControllers.Find(ExitingPlayerController) != INDEX_NONE)
	{
		AlivePlayerControllers.Remove(ExitingPlayerController);
		DeadPlayerControllers.Add(ExitingPlayerController);
	}
}

void AFFGameMode::BeginPlay()
{
	Super::BeginPlay();

	AFFGameState* FFGameState = GetGameState<AFFGameState>();
	if (IsValid(FFGameState))
	{
		FFGameState->SetGameState(EGameState::Waiting);
	}
}

AActor* AFFGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<APlayerStart*> AvailablePlayerStarts;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* PlayerStart = *It;

		if (IsPlayerStartAvailable(PlayerStart))
		{
			AvailablePlayerStarts.Add(PlayerStart);
		}
	}

	checkf(AvailablePlayerStarts.IsEmpty() == false, TEXT("There is no available Vehicle PlayerStart."));

	const int32 RandomIndex = FMath::RandHelper(AvailablePlayerStarts.Num());
	return Cast<AActor>(AvailablePlayerStarts[RandomIndex]);
}

void AFFGameMode::StartWaitingCountdown()
{
	AFFGameState* FFGameState = GetGameState<AFFGameState>();
	if (!IsValid(FFGameState))
	{
		return;
	}

	bWaitingCountdownStarted = true;

	FFGameState->SetGameState(EGameState::Waiting);
	FFGameState->SetRemainingTime(5);

	FString Message = FString::Printf(TEXT("게임 대기 중\n(남은 시간 : %02d초)"), FFGameState->GetRemainingTime());
	FFGameState->SetNotificationText(FText::FromString(Message));

	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AFFGameMode::HandleWaitingTimerTick, 1.0f, true);
}

void AFFGameMode::HandleWaitingTimerTick()
{
	AFFGameState* FFGameState = GetGameState<AFFGameState>();
	if (!IsValid(FFGameState))
	{
		return;
	}

	const int32 NewRemainingTime = FFGameState->GetRemainingTime() - 1;
	FFGameState->SetRemainingTime(NewRemainingTime);

	FString Message = FString::Printf(TEXT("게임 대기 중\n(남은 시간 : %02d초)"), FFGameState->GetRemainingTime());
	FFGameState->SetNotificationText(FText::FromString(Message));

	if (NewRemainingTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(GameTimerHandle);
		StartPlayingState();
	}
}

void AFFGameMode::StartPlayingState()
{
	bWaitingCountdownStarted = true;

	AFFGameState* FFGameState = GetGameState<AFFGameState>();
	if (!IsValid(FFGameState))
	{
		return;
	}

	FFGameState->SetGameState(EGameState::Playing);
	FFGameState->SetRemainingTime(30);

	int32 Minutes = FFGameState->GetRemainingTime() / 60;
	int32 Seconds = FFGameState->GetRemainingTime() % 60;
	FString Message = FString::Printf(TEXT("게임 시작!\n(남은 시간 : %02d분 %02d초)"), Minutes, Seconds);
	FFGameState->SetNotificationText(FText::FromString(Message));

	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AFFGameMode::HandlePlayingTimerTick, 1.0f, true);

}

void AFFGameMode::HandlePlayingTimerTick()
{
	AFFGameState* FFGameState = GetGameState<AFFGameState>();
	if (!IsValid(FFGameState))
	{
		return;
	}

	const int32 NewRemainingTime = FFGameState->GetRemainingTime() - 1;
	FFGameState->SetRemainingTime(NewRemainingTime);

	int32 Minutes = FFGameState->GetRemainingTime() / 60;
	int32 Seconds = FFGameState->GetRemainingTime() % 60;
	FString Message = FString::Printf(TEXT("게임 시작!\n(남은 시간 : %02d분 %02d초)"), Minutes, Seconds);
	FFGameState->SetNotificationText(FText::FromString(Message));

	if (NewRemainingTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(GameTimerHandle);
		FinishGame();
	}
}

void AFFGameMode::FinishGame()
{
	AFFGameState* FFGameState = GetGameState<AFFGameState>();
	if (!IsValid(FFGameState))
	{
		return;
	}

	int32 HighestCoins = -1;
	TArray<AFFPlayerState*> Winners;

	for (APlayerState* PlayerState : FFGameState->PlayerArray)
	{
		AFFPlayerState* FFPlayerState = Cast<AFFPlayerState>(PlayerState);
		if (IsValid(FFPlayerState))
		{
			const int32 PlayerCoins = FFPlayerState->GetCoins();

			if (HighestCoins < PlayerCoins)
			{
				HighestCoins = PlayerCoins;
				Winners.Empty();
				Winners.Add(FFPlayerState);
			}
			else if (HighestCoins == PlayerCoins)
			{
				Winners.Add(FFPlayerState);
			}
		}
	}

	FFGameState->SetWinners(Winners);
	FFGameState->SetGameState(EGameState::Finished);
	FFGameState->SetRemainingTime(0);
	FFGameState->SetNotificationText(FText::FromString(TEXT("")));

	FString WinnerNames;

	for (const TObjectPtr<AFFPlayerState>& Winner : Winners)
	{
		if (!IsValid(Winner))
		{
			continue;
		}

		if (!WinnerNames.IsEmpty())
		{
			WinnerNames += TEXT(", ");
		}

		WinnerNames += Winner->GetPlayerNameString();
	}

	FString Message;

	if (WinnerNames.IsEmpty())
	{
		Message = TEXT("우승자가 없습니다.");
	}
	else
	{
		Message = FString::Printf(TEXT("게임이 종료되었습니다!\n(우승자: %s"), *WinnerNames);
	}

	Message += ")";

	FFGameState->SetNotificationText(FText::FromString(Message));
}

bool AFFGameMode::IsPlayerStartAvailable(const APlayerStart* PlayerStart) const
{
	if (IsValid(PlayerStart) == false || IsValid(GetWorld()) == false)
	{
		return false;
	}

	const FTransform PlayerStartTransform = PlayerStart->GetActorTransform();
	const FVector CheckLocation = PlayerStartTransform.TransformPosition(VehicleSpawnCheckOffset);
	const FQuat CheckRotation = PlayerStartTransform.GetRotation();
	const FCollisionShape CheckShape = FCollisionShape::MakeBox(VehicleSpawnCheckHalfExtent);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Vehicle);

	const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(VehiclePlayerStartCheck), false);

	const bool bVehicleExists =
		GetWorld()->OverlapAnyTestByObjectType(
			CheckLocation,
			CheckRotation,
			ObjectQueryParams,
			CheckShape,
			QueryParams
		);

	return !bVehicleExists;
}
