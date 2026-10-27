
#pragma once

#include "CoreMinimal.h"
#include "FFItemBase.h"
#include "FFMineItem.generated.h"

class USceneComponent;
class USphereComponent;
class UStaticMeshComponent;
class UParticleSystem;
class USoundBase;

UCLASS()
class FASTANDFURIOUS_API AFFMineItem : public AFFItemBase
{
	GENERATED_BODY()

public:
	AFFMineItem();

	virtual void OnOverlapSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;


protected:
	virtual void PickupAction(APawn* VehiclePawn) override;
};
