
#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
{
	ExplosionDelay = 2.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamege = 30.0f;
	ItemType = "Mine";
	bHasExploded = false;

	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollsion"));
	ExplosionCollision->InitSphereRadius(ExplosionRadius);
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision->SetupAttachment(Scene);
}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded) return;

	Super::ActivateItem(Activator);

	GetWorld()->GetTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&AMineItem::Explode,
		ExplosionDelay,
		false
	);
}

void AMineItem::Explode()
{
	if (bHasExploded) return;
	bHasExploded = true;

	UParticleSystemComponent* Particle = nullptr;

	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			false
		);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
		);
	}

	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(
				Actor,
				ExplosionDamege,
				nullptr,
				this,
				UDamageType::StaticClass()
			);
		}
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		ExplosionCleanupTimerHandle,
		this,
		&AMineItem::DestroyItem,
		2.0f,
		false
	);

	if (Particle && IsValid(Particle))
	{
		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

		FTimerHandle DestroyParticleTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				AsyncTask(ENamedThreads::GameThread, [WeakParticle]()
				{
					if (WeakParticle.IsValid())
					{
						WeakParticle.Get()->DestroyComponent();
					}
				});
			},
			1.0f,
			false
		);
	}
}