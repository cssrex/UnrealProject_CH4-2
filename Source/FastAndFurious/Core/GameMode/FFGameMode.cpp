
#include "FFGameMode.h"
#include "Player/PlayerController/FFPlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Core/Subsystem/FFChatSubsystem.h"
#include "Player/PlayerState/FFPlayerState.h"
#include "Core/GameState/FFGameStateBase.h"

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

	if (AFFPlayerController* FFPlayerController = Cast<AFFPlayerController>(NewPlayer))
	{
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

}

void AFFGameMode::HandleWaitingTimerTick()
{
}

void AFFGameMode::StartPlayingState()
{
}

void AFFGameMode::HandlePlayingTimerTick()
{
}

void AFFGameMode::FinishGame()
{
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
