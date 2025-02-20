#pragma once

#include "CoreMinimal.h"
#include "CoinItem.h"
#include "BIgCoinItem.generated.h"


UCLASS()
class REPAIR_API ABIgCoinItem : public ACoinItem
{
	GENERATED_BODY()

public:
	ABIgCoinItem();

	virtual void ActivateItem(AActor* Activator) override;
	
};
