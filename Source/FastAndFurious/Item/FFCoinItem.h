
#pragma once

#include "CoreMinimal.h"
#include "FFItemBase.h"
#include "FFCoinItem.generated.h"

UCLASS()
class FASTANDFURIOUS_API AFFCoinItem : public AFFItemBase
{
	GENERATED_BODY()

public:
	AFFCoinItem();

protected:
	virtual void PickupAction(APawn* VehiclePawn) override;
};
