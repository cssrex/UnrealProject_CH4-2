
#include "FFPawn.h"
#include "Player/Wheel/FFWheelFront.h"
#include "Player/Wheel/FFWheelRear.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Item/FFMineItem.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Player/PlayerState/FFPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameState/FFGameState.h"
#include "GameFramework/DamageType.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "VehiclePawn"

DEFINE_LOG_CATEGORY(LogTemplateVehicle);

AFFPawn::AFFPawn()
{
	FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Front Spring Arm"));
	FrontSpringArm->SetupAttachment(GetMesh());
	FrontSpringArm->TargetArmLength = 0.0f;
	FrontSpringArm->bDoCollisionTest = false;
	FrontSpringArm->bEnableCameraRotationLag = true;
	FrontSpringArm->CameraRotationLagSpeed = 15.0f;
	FrontSpringArm->SetRelativeLocation(FVector(30.0f, 0.0f, 120.0f));

	FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Front Camera"));
	FrontCamera->SetupAttachment(FrontSpringArm);
	FrontCamera->bAutoActivate = false;

	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(GetMesh());
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	BackSpringArm->bDoCollisionTest = false;
	BackSpringArm->bInheritPitch = false;
	BackSpringArm->bInheritRoll = false;
	BackSpringArm->bEnableCameraRotationLag = true;
	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));

	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());

}

void AFFPawn::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority() && !IsLocallyControlled())
	{
		return;
	}

	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return;
	}

	UpdateGameStateVehicleLock();

	World->GameStateSetEvent.AddUObject(this, &AFFPawn::OnGameStateSet);

	OnGameStateSet(World->GetGameState());
}

void AFFPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GameStateSetEvent.RemoveAll(this);

		if (AFFGameState* FFGameState = World->GetGameState<AFFGameState>())
		{
			FFGameState->OnGameStateChanged.RemoveAll(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AFFPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFFPawn, bThrottleBlocked);
}

void AFFPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AFFPawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &AFFPawn::Steering);

		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AFFPawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &AFFPawn::Throttle);

		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &AFFPawn::Brake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &AFFPawn::StartBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &AFFPawn::StopBrake);

		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &AFFPawn::StartHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AFFPawn::StopHandbrake);

		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &AFFPawn::LookAround);

		EnhancedInputComponent->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &AFFPawn::ToggleCamera);

		EnhancedInputComponent->BindAction(ResetVehicleAction, ETriggerEvent::Triggered, this, &AFFPawn::ResetVehicle);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AFFPawn::InputAttack);

		EnhancedInputComponent->BindAction(SpawnMineAction, ETriggerEvent::Started, this, &AFFPawn::SpawnMine);
	}
	else
	{
		UE_LOG(LogTemplateVehicle, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFFPawn::Tick(float Delta)
{
	Super::Tick(Delta);

	LimitForwardSpeed();

	bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

	float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;
	CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);

	BackSpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));
}

float AFFPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority())
	{
		return 0.0f;
	}

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage <= 0)
	{
		return ActualDamage;
	}

	if (DamageCauser == this)
	{
		return 0.0f;
	}

	AFFPawn* Attacker = Cast<AFFPawn>(DamageCauser);
	if (!IsValid(Attacker))
	{
		return 0.0f;
	}

	// 맞은 사람
	AFFPlayerState* VictimPlayerState = GetPlayerState<AFFPlayerState>();

	// 때린 사람
	AFFPlayerState* AttackerPlayerState = Attacker->GetPlayerState<AFFPlayerState>();

	if (!IsValid(VictimPlayerState) || !IsValid(AttackerPlayerState))
	{
		return 0.0f;
	}

	const int32 TransferCoinAmount = (int32)ActualDamage;
	if (VictimPlayerState->TryRemoveCoins(TransferCoinAmount))
	{
		UE_LOG(LogTemplateVehicle, Warning, TEXT("%s Transferred %d coins to %s"), *GetNameSafe(VictimPlayerState), TransferCoinAmount, *GetNameSafe(AttackerPlayerState));
		AttackerPlayerState->AddCoins(TransferCoinAmount);
	}
	else
	{
		UE_LOG(LogTemplateVehicle, Warning, TEXT("%s has no coin to transfer"), *GetNameSafe(VictimPlayerState), TransferCoinAmount, *GetNameSafe(AttackerPlayerState));
	}

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);

		const FHitResult& HitInfo = PointDamageEvent.HitInfo;
		const FName HitBone = HitInfo.BoneName;

		const bool bHitWheel =
			// 스포츠카
			HitBone == FName(TEXT("Phys_Wheel_FL")) ||
			HitBone == FName(TEXT("Phys_Wheel_FR")) ||
			HitBone == FName(TEXT("Phys_Wheel_BL")) ||
			HitBone == FName(TEXT("Phys_Wheel_BR")) ||

			// 오프로드 차량
			HitBone == FName(TEXT("PhysWheel_FL")) ||
			HitBone == FName(TEXT("PhysWheel_FR")) ||
			HitBone == FName(TEXT("PhysWheel_BL")) ||
			HitBone == FName(TEXT("PhysWheel_BR"));

		UE_LOG(LogTemplateVehicle, Warning, TEXT("Hit Bone: %s / Wheel Hit: %s"), *HitBone.ToString(), bHitWheel ? TEXT("Yes") : TEXT("No"));

		if (bHitWheel)
		{
			// TODO: 바퀴 적중 시 원하는 처리

			VictimPlayerState->AddMaxSpeedPenalty(50.0f);

			if (!GetWorldTimerManager().IsTimerActive(PenaltyTimer))
			{
				GetWorldTimerManager().SetTimer(PenaltyTimer, this, &AFFPawn::OnApplyPenalty, 1.0f, true);
			}

		}
	}

	return ActualDamage;
}

void AFFPawn::ApplyMineEffect()
{
	if (!HasAuthority())
	{
		return;
	}

	bThrottleBlocked = true;

	if (IsValid(ChaosVehicleMovement))
	{
		ChaosVehicleMovement->SetThrottleInput(0.0f);
	}

	GetWorldTimerManager().SetTimer(MineEffectTimerHandle, this, &AFFPawn::EndMineEffect, 2.0f, false);
}

void AFFPawn::Steering(const FInputActionValue& Value)
{
	if (!IsGamePlaying())
	{
		ChaosVehicleMovement->SetSteeringInput(0.0f);
		return;
	}

	float SteeringValue = Value.Get<float>();

	ChaosVehicleMovement->SetSteeringInput(SteeringValue);
}

void AFFPawn::Throttle(const FInputActionValue& Value)
{
	if (!IsGamePlaying() || bThrottleBlocked)
	{
		ChaosVehicleMovement->SetThrottleInput(0.0f);
		return;
	}

	float ThrottleValue = Value.Get<float>();

	ChaosVehicleMovement->SetThrottleInput(ThrottleValue);
}

void AFFPawn::Brake(const FInputActionValue& Value)
{
	if (!IsGamePlaying())
	{
		ChaosVehicleMovement->SetBrakeInput(0.0f);
		return;
	}

	float BreakValue = Value.Get<float>();

	ChaosVehicleMovement->SetBrakeInput(BreakValue);
}

void AFFPawn::StartBrake(const FInputActionValue& Value)
{
	BrakeLights(true);
}

void AFFPawn::StopBrake(const FInputActionValue& Value)
{
	BrakeLights(false);

	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void AFFPawn::StartHandbrake(const FInputActionValue& Value)
{
	if (!IsGamePlaying())
	{
		return;
	}

	ChaosVehicleMovement->SetHandbrakeInput(true);

	BrakeLights(true);
}

void AFFPawn::StopHandbrake(const FInputActionValue& Value)
{
	if (!IsGamePlaying())
	{
		ChaosVehicleMovement->SetHandbrakeInput(true);
		return;
	}

	ChaosVehicleMovement->SetHandbrakeInput(false);

	BrakeLights(false);
}

void AFFPawn::LookAround(const FInputActionValue& Value)
{
	float LookValue = Value.Get<float>();

	BackSpringArm->AddLocalRotation(FRotator(0.0f, LookValue, 0.0f));
}

void AFFPawn::ToggleCamera(const FInputActionValue& Value)
{
	bFrontCameraActive = !bFrontCameraActive;

	FrontCamera->SetActive(bFrontCameraActive);
	BackCamera->SetActive(!bFrontCameraActive);
}

void AFFPawn::ResetVehicle(const FInputActionValue& Value)
{
	if (!IsGamePlaying())
	{
		return;
	}

	FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

	FRotator ResetRotation = GetActorRotation();
	ResetRotation.Pitch = 0.0f;
	ResetRotation.Roll = 0.0f;

	SetActorTransform(FTransform(ResetRotation, ResetLocation, FVector::OneVector), false, nullptr, ETeleportType::TeleportPhysics);

	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);

	UE_LOG(LogTemplateVehicle, Error, TEXT("Reset Vehicle"));
}

void AFFPawn::InputAttack(const FInputActionValue& Value)
{
	if (!IsGamePlaying())
	{
		return;
	}

	ServerRPCAttack();
}

void AFFPawn::SpawnMine(const FInputActionValue& Value)
{
	if (!IsGamePlaying())
	{
		return;
	}

	ServerRPCSpawnMine();
}

void AFFPawn::ServerRPCSpawnMine_Implementation()
{
	if (!IsGamePlaying())
	{
		return;
	}

	UE_LOG(LogTemplateVehicle, Warning, TEXT("ServerRPCSpawnMine called"));

	UWorld* World = GetWorld();

	if (IsValid(World))
	{
		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * -200.f;
		FRotator SpawnRotation = GetActorRotation();
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this;

		AActor* SpawnedMine = World->SpawnActor<AActor>(MineClass, SpawnLocation, SpawnRotation, Params);

		UE_LOG(LogTemplateVehicle, Warning, TEXT("Mine spawn result: %s / Location: %s"), *GetNameSafe(SpawnedMine), *SpawnLocation.ToString());
	}
}

void AFFPawn::OnApplyPenalty()
{
	AFFPlayerState* FFPlayerState = GetPlayerState<AFFPlayerState>();

	if (!IsValid(FFPlayerState))
	{
		GetWorldTimerManager().ClearTimer(PenaltyTimer);
		return;
	}

	FFPlayerState->RemoveMaxSpeedPenalty(10.0f);

	if (FFPlayerState->GetMaxSpeedPenalty() <= 0.0f)
	{
		GetWorldTimerManager().ClearTimer(PenaltyTimer);
	}
}

void AFFPawn::LimitForwardSpeed()
{
	// 서버가 차량의 최종 물리 상태를 관리
	if (!HasAuthority() || !IsValid(GetMesh()))
	{
		return;
	}

	float BonusSpeedKmh = 0.0f;
	float MaxSpeedPenaltyKmh = 0.0f;

	if (AFFPlayerState* FFPlayerState = GetPlayerState<AFFPlayerState>())
	{
		BonusSpeedKmh = FFPlayerState->GetBonusMaxSpeed();
		MaxSpeedPenaltyKmh = FFPlayerState->GetMaxSpeedPenalty();
	}

	// 기본 속도와 보너스는 km/h 단위
	const float MaxSpeedKmh = FMath::Max(10.0f, BaseMaxSpeedKmh + BonusSpeedKmh - MaxSpeedPenaltyKmh);

	// 물리 속도는 cm/s이므로 변환
	const float MaxSpeedCms = MaxSpeedKmh / 0.036f;

	const FVector Velocity = GetMesh()->GetPhysicsLinearVelocity();
	const FVector ForwardDirection = GetActorForwardVector();

	// 현재 속도 중 차량 전방 방향의 성분
	const float ForwardSpeed = FVector::DotProduct(Velocity, ForwardDirection);

	if (ForwardSpeed > MaxSpeedCms)
	{
		const float ExcessSpeed = ForwardSpeed - MaxSpeedCms;

		// 전방 속도에서 초과한 만큼만 제거
		const FVector LimitedVelocity = Velocity - ForwardDirection * ExcessSpeed;

		GetMesh()->SetPhysicsLinearVelocity(LimitedVelocity);
	}
}

void AFFPawn::OnRep_ThrottleBlocked()
{
	if (bThrottleBlocked && IsValid(ChaosVehicleMovement))
	{
		ChaosVehicleMovement->SetThrottleInput(0.0f);
	}
}

void AFFPawn::EndMineEffect()
{
	bThrottleBlocked = false;
}

void AFFPawn::OnGameStateSet(AGameStateBase* GameState)
{
	AFFGameState* FFGameState = Cast<AFFGameState>(GameState);

	if (!IsValid(FFGameState))
	{
		return;
	}

	if (!FFGameState->OnGameStateChanged.IsBoundToObject(this))
	{
		FFGameState->OnGameStateChanged.AddUObject(this, &AFFPawn::UpdateGameStateVehicleLock);
	}

	UpdateGameStateVehicleLock();
}

bool AFFPawn::IsGamePlaying() const
{
	const UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return false;
	}

	const AFFGameState* FFGameState = World->GetGameState<AFFGameState>();

	if (!IsValid(FFGameState))
	{
		return false;
	}

	return FFGameState->GetCurrentGameState() == EGameState::Playing;
}

void AFFPawn::UpdateGameStateVehicleLock()
{
	if (!IsValid(ChaosVehicleMovement))
	{
		return;
	}

	if (!HasAuthority() && !IsLocallyControlled())
	{
		return;
	}

	if (!IsGamePlaying())
	{
		ChaosVehicleMovement->SetThrottleInput(0.0f);
		ChaosVehicleMovement->SetSteeringInput(0.0f);
		ChaosVehicleMovement->SetBrakeInput(0.0f);
		ChaosVehicleMovement->SetHandbrakeInput(true);

		bGameStateHandbrakeLocked = true;
	}
	else if (bGameStateHandbrakeLocked)
	{
		ChaosVehicleMovement->SetHandbrakeInput(false);

		bGameStateHandbrakeLocked = false;
	}
}

void AFFPawn::ServerRPCAttack_Implementation()
{
	if (!IsGamePlaying())
	{
		return;
	}

	UE_LOG(LogTemplateVehicle, Warning, TEXT("Attack"));

	const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
	const FVector End = Start + GetActorForwardVector() * 700.0f;

	if (UWorld* World = GetWorld())
	{
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		bool bHitVehicle = false;
		bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
		if (bHit)
		{
			AActor* HitActor = HitResult.GetActor();

			if (IsValid(HitActor))
			{
				UE_LOG(LogTemplateVehicle, Warning, TEXT("Hit Actor: %s"), *GetNameSafe(HitResult.GetActor()));
			}

			if (AFFPawn* FFPawn = Cast<AFFPawn>(HitActor))
			{
				bHitVehicle = true;
				// UGameplayStatics::ApplyDamage(FFPawn, 5.0f, GetController(), this, UDamageType::StaticClass());

				const FVector ShotDirection = (End - Start).GetSafeNormal();

				UGameplayStatics::ApplyPointDamage(FFPawn, 5.0f, (End - Start).GetSafeNormal(), HitResult, GetController(), this, UDamageType::StaticClass());
			}
		}

		MulticastRPCPlayAttackEffect(Start, End, bHitVehicle, HitResult.ImpactPoint, HitResult.ImpactNormal);
	}
}

void AFFPawn::MulticastRPCPlayAttackEffect_Implementation(const FVector& TraceStart, const FVector& TraceEnd, bool bHitVehicle, const FVector& ImpactPoint, const FVector& ImpactNormal)
{
	if (UWorld* World = GetWorld())
	{
		FColor LineColor = bHitVehicle ? FColor::Green : FColor::Red;
		DrawDebugLine(World, TraceStart, TraceEnd, LineColor, false, 2.0f, 0, 5.0f);

		if (bHitVehicle)
		{
			if (AttackEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, AttackEffect, ImpactPoint, ImpactNormal.Rotation());
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE