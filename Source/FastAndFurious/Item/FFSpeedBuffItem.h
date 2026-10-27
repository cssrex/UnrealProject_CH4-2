
#pragma once

#include "CoreMinimal.h"
#include "FFItemBase.h"
#include "FFSpeedBuffItem.generated.h"

class USceneComponent;
class USphereComponent;
class UStaticMeshComponent;
class UParticleSystem;
class USoundBase;

UCLASS()
class FASTANDFURIOUS_API AFFSpeedBuffItem : public AFFItemBase
{
	GENERATED_BODY()

public:
	AFFSpeedBuffItem();

protected:
	virtual void PickupAction(APawn* VehiclePawn) override;
};
