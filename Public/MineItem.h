
#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "MineItem.generated.h"


UCLASS()
class REPAIR_API AMineItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AMineItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* ExplosionCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* ExplosionParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ExplosionDamege;

	bool bHasExploded;
	FTimerHandle ExplosionTimerHandle;
	UPROPERTY()
	FTimerHandle ExplosionCleanupTimerHandle;

	virtual void ActivateItem(AActor* Activator) override;

	void Explode();
};
