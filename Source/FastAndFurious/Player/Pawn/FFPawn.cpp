
#include "FFPawn.h"
#include "Player/Wheel/FFWheelFront.h"
#include "Player/Wheel/FFWheelRear.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Player/PlayerState/FFPlayerState.h"
#include "Kismet/GameplayStatics.h"

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
	}
	else
	{
		UE_LOG(LogTemplateVehicle, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFFPawn::Tick(float Delta)
{
	Super::Tick(Delta);

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

	return ActualDamage;
}

void AFFPawn::Steering(const FInputActionValue& Value)
{
	float SteeringValue = Value.Get<float>();

	ChaosVehicleMovement->SetSteeringInput(SteeringValue);
}

void AFFPawn::Throttle(const FInputActionValue& Value)
{
	float ThrottleValue = Value.Get<float>();

	ChaosVehicleMovement->SetThrottleInput(ThrottleValue);
}

void AFFPawn::Brake(const FInputActionValue& Value)
{
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
	ChaosVehicleMovement->SetHandbrakeInput(true);

	BrakeLights(true);
}

void AFFPawn::StopHandbrake(const FInputActionValue& Value)
{
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
	ServerRPCAttack();
}

void AFFPawn::ServerRPCAttack_Implementation()
{
	UE_LOG(LogTemplateVehicle, Warning, TEXT("Attack"));

	const FVector Start = GetActorLocation();
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
				UGameplayStatics::ApplyDamage(FFPawn, 5.0f, GetController(), this, UDamageType::StaticClass());
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