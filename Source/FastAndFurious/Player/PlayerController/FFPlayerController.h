
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FFPlayerController.generated.h"

class UInputMappingContext;
class AFFPawn;
class UFFUI;

UCLASS(abstract)
class FASTANDFURIOUS_API  AFFPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	TObjectPtr<AFFPawn> VehiclePawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	TSubclassOf<UFFUI> VehicleUIClass;

	TObjectPtr<UFFUI> VehicleUI;

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:

	virtual void Tick(float Delta) override;

protected:

	virtual void OnPossess(APawn* InPawn) override;
};
