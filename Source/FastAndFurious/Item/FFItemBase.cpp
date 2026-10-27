
#include "Item/FFItemBase.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "Player/Pawn/FFPawn.h"

AFFItemBase::AFFItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	NetUpdateFrequency = 1.f; // 초당 몇번 Replication 할지
	MinNetUpdateFrequency = 1.f; // 초당 최소 몇번 Replication 할지

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(SceneRoot);
	OverlapSphere->SetSphereRadius(100.f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	OverlapSphere->SetGenerateOverlapEvents(true);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(SceneRoot);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFFItemBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) // 서버에서만 충돌 판정
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AFFItemBase::OnOverlapSphereBeginOverlap);
	}
}

void AFFItemBase::OnOverlapSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) // 서버가 아니면 return (즉, 판정 안함)
	{
		return;
	}

	AFFPawn* VehiclePawn = Cast<AFFPawn>(OtherActor);
	if (IsValid(VehiclePawn) == false)
	{
		return;
	}

	PickupAction(VehiclePawn);

	MulticastRPCPickupEffects();

	Destroy();
}

void AFFItemBase::PickupAction(APawn* VehiclePawn)
{

}

void AFFItemBase::MulticastRPCPickupEffects_Implementation()
{
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		return;
	}

	if (IsValid(PickupParticle))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupParticle, GetActorLocation(), FRotator::ZeroRotator, true);
	}

	if (IsValid(PickupSound))
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
	}
}


