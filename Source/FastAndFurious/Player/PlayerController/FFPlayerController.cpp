
#include "FFPlayerController.h"
#include "Player/Pawn/FFPawn.h"
#include "UI/FFUI.h"
#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "EngineUtils.h"
#include "Core/Subsystem/FFChatSubsystem.h"
#include "Player/PlayerState/FFPlayerState.h"
#include "EnhancedInputComponent.h"

void AFFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	VehicleUI = CreateWidget<UFFUI>(this, VehicleUIClass);

	check(VehicleUI);

	VehicleUI->AddToViewport();

	if (UFFChatSubsystem* ChatSubsystem = GetWorld()->GetSubsystem<UFFChatSubsystem>())
	{
		for (const FChatData& ChatData : ChatSubsystem->GetChatLog())
		{
			VehicleUI->OnNewChatMessage(ChatData.Sender, ChatData.Message);
		}
	}
}

void AFFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (IsValid(OpenChatAction))
		{
			EnhancedInputComponent->BindAction(OpenChatAction, ETriggerEvent::Started, this, &AFFPlayerController::OpenChat);
		}
	}
}

void AFFPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, VehiclePawn);
}

void AFFPlayerController::Tick(float Delta)
{
	Super::Tick(Delta);

	if (IsValid(VehiclePawn) && IsValid(VehicleUI))
	{
		VehicleUI->UpdateSpeed(VehiclePawn->GetChaosVehicleMovement()->GetForwardSpeed());
		VehicleUI->UpdateGear(VehiclePawn->GetChaosVehicleMovement()->GetCurrentGear());
	}
}

void AFFPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	VehiclePawn = CastChecked<AFFPawn>(InPawn);
}

void AFFPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (IsValid(VehicleUI))
	{
		VehicleUI->InitializePlayerState();
	}
}

void AFFPlayerController::OpenChat()
{
	if (!IsLocalController() || !IsValid(VehicleUI))
	{
		return;
	}

	VehicleUI->FocusChatInput();
}

void AFFPlayerController::ServerRPCChatMessage_Implementation(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	AFFPlayerState* FFPlayerState = Cast<AFFPlayerState>(PlayerState);
	if (!IsValid(FFPlayerState))
	{
		return;
	}

	const FString PlayerName = FFPlayerState->GetPlayerNameString();

	if (UFFChatSubsystem* ChatSubsystem = GetWorld()->GetSubsystem<UFFChatSubsystem>())
	{
		ChatSubsystem->AddChatMessage(PlayerName, Message);
	}

	for (TActorIterator<AFFPlayerController> It(GetWorld()); It; ++It)
	{
		if (AFFPlayerController* PC = *It)
		{
			PC->ClientRPCChatMessage(PlayerName, Message);
		}
	}
}

void AFFPlayerController::ClientRPCChatMessage_Implementation(const FString& Sender, const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *Sender, *Message);

	VehicleUI->OnNewChatMessage(Sender, Message);
}

void AFFPlayerController::ClientRPCChatLog_Implementation(const TArray<FChatData>& ChatLog)
{
	if (UFFChatSubsystem* ChatSubsystem = GetWorld()->GetSubsystem<UFFChatSubsystem>())
	{
		ChatSubsystem->SetChatLog(ChatLog);
	}
}
