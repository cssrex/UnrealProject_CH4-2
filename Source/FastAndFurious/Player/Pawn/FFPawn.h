
#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "FFPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UChaosWheeledVehicleMovementComponent;
class UParticleSystem;
class AFFMineItem;
class AGameStateBase;
struct FInputActionValue;
enum class EGameState : uint8;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateVehicle, Log, All);

UCLASS(abstract)
class AFFPawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FrontSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FrontCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* BackSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* BackCamera;

	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SteeringAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrottleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BrakeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* HandbrakeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAroundAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ToggleCameraAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ResetVehicleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SpawnMineAction;

	bool bFrontCameraActive = false;

	UPROPERTY(EditAnywhere, Category = Effects)
	UParticleSystem* AttackEffect;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle|Speed")
	float BaseMaxSpeedKmh = 100.0f;

	FTimerHandle PenaltyTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawner)
	TSubclassOf<AFFMineItem> MineClass;

	UPROPERTY(ReplicatedUsing = OnRep_ThrottleBlocked)
	bool bThrottleBlocked = false;

	FTimerHandle MineEffectTimerHandle;

	bool bGameStateHandbrakeLocked = false;

public:
	AFFPawn();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	virtual void Tick(float Delta) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	void ApplyMineEffect();

protected:

	void Steering(const FInputActionValue& Value);

	void Throttle(const FInputActionValue& Value);

	void Brake(const FInputActionValue& Value);

	void StartBrake(const FInputActionValue& Value);
	void StopBrake(const FInputActionValue& Value);

	void StartHandbrake(const FInputActionValue& Value);
	void StopHandbrake(const FInputActionValue& Value);

	void LookAround(const FInputActionValue& Value);

	void ToggleCamera(const FInputActionValue& Value);

	void ResetVehicle(const FInputActionValue& Value);

	void InputAttack(const FInputActionValue& Value);

	void SpawnMine(const FInputActionValue& Value);

	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle")
	void BrakeLights(bool bBraking);

protected:
	UFUNCTION(Server, Reliable)
	void ServerRPCAttack();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPlayAttackEffect(const FVector& TraceStart, const FVector& TraceEnd, bool bHitVehicle, const FVector& ImpactPoint, const FVector& ImpactNormal);

	UFUNCTION(Server, Reliable)
	void ServerRPCSpawnMine();

	UFUNCTION()
	void OnApplyPenalty();

	void LimitForwardSpeed();

	UFUNCTION()
	void OnRep_ThrottleBlocked();

	void EndMineEffect();

protected:
	void OnGameStateSet(AGameStateBase* GameState);

	bool IsGamePlaying() const;

	void UpdateGameStateVehicleLock();



public:
	FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return FrontSpringArm; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FrontCamera; }
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }
	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }
	FORCEINLINE const TObjectPtr<UChaosWheeledVehicleMovementComponent>& GetChaosVehicleMovement() const { return ChaosVehicleMovement; }
};
