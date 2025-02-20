
#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "DebuffItem.generated.h"


UCLASS()
class REPAIR_API ADebuffItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	ADebuffItem();

protected:
	virtual void ActivateItem(AActor* Activator) override;

private:
	UPROPERTY(EditAnywhere, Category = "Debuff")
	float SpeedMultiplier = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Debuff")
	float DebuffDuration = 3.0f;

	void ResetSpeed(ACharacter* AffectedActor);

	FTimerHandle DebuffTimerHandle;
};
