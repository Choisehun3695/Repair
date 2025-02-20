
#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CoreMinimal.h"
#include "Particles/ParticleSystemComponent.h"
#include "Async/Async.h"


ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Collision);

	//이벤트 바인딩
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnItemEndOverlap);

	PickupSound = nullptr;
	PickupParticle = nullptr;
}

void ABaseItem::OnItemOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Overlap!!!")));
		ActivateItem(OtherActor);
	}
}

void ABaseItem::OnItemEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

void ABaseItem::ActivateItem(AActor* Activator)
{
	if (!Activator) return;

	UParticleSystemComponent* Particle = nullptr;

	if (PickupParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			PickupParticle,
			GetActorLocation(),
			GetActorRotation(),
			true
		);
	}

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			PickupSound,
			GetActorLocation()
		);
	}

	if (Particle && IsValid(Particle))
	{
		FTimerHandle DestroyParticleTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[Particle]()
			{
				if (Particle && IsValid(Particle))
				{
					Particle->DestroyComponent();
				}
			},
			1.0f,
			false
		);
	}

	if (!OnDestroyed.IsAlreadyBound(this, &ABaseItem::ClearTimers))
	{
		OnDestroyed.AddDynamic(this, &ABaseItem::ClearTimers);
	}
}

void ABaseItem::RequestDestroyItem()
{
	if (!GetWorld()) return;

	FTimerHandle DestroyItemTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		DestroyItemTimerHandle,
		this,
		&ABaseItem::DestroyItem,
		1.0f,
		false
	);
}

void ABaseItem::ClearTimers(AActor* DestroyedActor)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

FName ABaseItem::GetItemType() const {
	return ItemType;
}


void ABaseItem::DestroyItem()
{
	if (!IsInGameThread())
	{
		AsyncTask(ENamedThreads::GameThread, [this]()
			{
				Destroy();
			});
	}
	else
	{
		Destroy();
	}
}

