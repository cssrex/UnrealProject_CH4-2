
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FFCoinItem.generated.h"

class USceneComponent;
class USphereComponent;
class UStaticMeshComponent;
class UParticleSystem;
class USoundBase;

UCLASS()
class FASTANDFURIOUS_API AFFCoinItem : public AActor
{
	GENERATED_BODY()

public:
	AFFCoinItem();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnOverlapSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPickupEffects(); // 코인을 먹었을 때의 소리 & 효과 재생

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	TObjectPtr<USceneComponent> SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	TObjectPtr<USphereComponent> OverlapSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	TObjectPtr<UStaticMeshComponent> ItemMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin")
	TObjectPtr<UParticleSystem> PickupParticle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coin")
	TObjectPtr<USoundBase> PickupSound;
};
