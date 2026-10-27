
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FFItemSpawner.generated.h"

class UBillboardComponent;

UCLASS()
class FASTANDFURIOUS_API AFFItemSpawner : public AActor
{
	GENERATED_BODY()

public:
	AFFItemSpawner();

protected:
	virtual void BeginPlay() override;

protected:
	void SpawnItem();

	UFUNCTION()
	void OnItemDestroyed(AActor* DestroyedActor);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawner)
	TSubclassOf<AActor> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawner)
	float SpawnInterval = 2.f;

protected:
	UPROPERTY()
	TObjectPtr<AActor> SpawnedItem;

	FTimerHandle SpawnTimerHandle;

protected:
	TObjectPtr<UBillboardComponent> BillboardComponent;

};
