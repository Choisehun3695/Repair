
#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SpartaGameState.h"
#include "ItemSpawnRow.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	SpawnCount = 10;
	ItemDataTable = nullptr;
}

void ASpawnVolume::SetSpawnCount(int32 Count)
{
	SpawnCount = Count;
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	if (!GetWorld() || !ItemDataTable) return nullptr;

	FItemSpawnRow* SelectedRow = GetRandomItem();
	if (!SelectedRow) return nullptr;

	TSubclassOf<AActor> ItemClass = SelectedRow->ItemClass;
	if (!ItemClass) return nullptr;

	ASpartaGameState* GameState = GetWorld()->GetGameState<ASpartaGameState>();
	if (GameState)
	{
		int32 CurrentWave = GameState->CurrentWave;
		int32 ItemsToSpawn = 10 + (CurrentWave * 5);
		SetSpawnCount(ItemsToSpawn);
	}

	
	return SpawnItem(ItemClass);
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	FVector SpawnLocation = GetRandomPointInVolume();
	FRotator SpawnRotation = FRotator::ZeroRotator;

	return GetWorld()->SpawnActor<AActor>(ItemClass, SpawnLocation, SpawnRotation);
}


FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable) return nullptr;

	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));

	ItemDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty()) return nullptr;

	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->Spawnchance;
		}
	}

	const float RandValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f;

	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->Spawnchance;
		if (RandValue <= AccumulateChance)
		{
			return Row;
		}
	}
	return nullptr;
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	// (200, 100, 50) (2,1,1) -> (400, 100, 50) Áß½Å > ³¡
	FVector BoxExtnet = SpawningBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtnet.X, BoxExtnet.X),
		FMath::FRandRange(-BoxExtnet.Y, BoxExtnet.Y),
		FMath::FRandRange(-BoxExtnet.Z, BoxExtnet.Z)
	);
}
