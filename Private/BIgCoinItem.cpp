
#include "BIgCoinItem.h"

ABIgCoinItem::ABIgCoinItem()
{
	PointValue = 50;
	ItemType = "BigCoin";
}

void ABIgCoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}