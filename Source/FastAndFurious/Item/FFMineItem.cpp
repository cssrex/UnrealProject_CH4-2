
#include "FFMineItem.h"

#include "Player/PlayerState/FFPlayerState.h"
#include "Player/Pawn/FFPawn.h"

AFFMineItem::AFFMineItem()
{
}

void AFFMineItem::OnOverlapSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) // 서버가 아니면 return (즉, 판정 안함)
	{
		return;
	}

	if (OtherActor == GetOwner())
	{
		return;
	}

	Super::OnOverlapSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AFFMineItem::PickupAction(APawn* VehiclePawn)
{
	AFFPawn* FFPawn = Cast<AFFPawn>(VehiclePawn);

	if (IsValid(FFPawn))
	{
		FFPawn->ApplyMineEffect();
	}
}

