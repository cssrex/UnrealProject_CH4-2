
#include "Item/FFItemSpawner.h"

#include "Components/BillboardComponent.h"

AFFItemSpawner::AFFItemSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	SetRootComponent(BillboardComponent);
}

void AFFItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SpawnItem();
	}
}

void AFFItemSpawner::SpawnItem()
{
	UWorld* World = GetWorld();

	SpawnedItem = World->SpawnActorDeferred<AActor>(ItemClass, GetActorTransform(), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (IsValid(SpawnedItem))
	{
		SpawnedItem->OnDestroyed.AddDynamic(this, &AFFItemSpawner::OnItemDestroyed);
		SpawnedItem->FinishSpawning(GetActorTransform());
	}
}

void AFFItemSpawner::OnItemDestroyed(AActor* DestroyedActor)
{
	if (!HasAuthority() || !IsValid(ItemClass))
	{
		return;
	}

	SpawnedItem = nullptr;

	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AFFItemSpawner::SpawnItem, SpawnInterval, false);

}


