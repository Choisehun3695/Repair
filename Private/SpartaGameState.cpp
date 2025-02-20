#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ASpartaGameState::ASpartaGameState()
{
    Score = 0;
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;
    CurrentLevelIndex = 0;
    MaxLevels = 4;

    CurrentWave = 1;
    MaxWaves = 3;
    WaveDuration = 60.0f;
}

void ASpartaGameState::BeginPlay()
{
    Super::BeginPlay();

    StartLevel();

    GetWorldTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &ASpartaGameState::UpdateHUD,
        0.1f,
        true
    );
}

int32 ASpartaGameState::GetScore() const
{
    return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            SpartaGameInstance->AddToScore(Amount);
        }
    }
}


void ASpartaGameState::StartLevel()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            SpartaPlayerController->ShowGameHUD();
        }
    }

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
        }
    }

    CurrentWave = 1;
    StartWave();
}

void ASpartaGameState::StartWave()
{
    if (CurrentWave > MaxWaves)
    {
        EndLevel();
        return;
    }

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Wave %d Start!"), CurrentWave));

    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;

    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

    int32 ItemsToSpawn = 20 + (CurrentWave * 5);

    if (FoundVolumes.Num() > 0)
    {
        ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
        if (SpawnVolume)
        {
            for (int32 i = 0; i < ItemsToSpawn; i++)
            {
                AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
                if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
                {
                    SpawnedCoinCount++;
                }
            }
        }
    }

    GetWorldTimerManager().SetTimer(
        LevelTimerHandle,
        this,
        &ASpartaGameState::EndWave,
        WaveDuration,
        false
    );
}

void ASpartaGameState::EndWave()
{
    CurrentWave++;

    if (CurrentWave > MaxWaves)
    {
        EndLevel();
    }
    else
    {
        StartWave();
    }
}

void ASpartaGameState::OnCoinCollected()
{
    CollectedCoinCount++;

    if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
    {
        EndWave();
    }
}

void ASpartaGameState::EndLevel()
{
    GetWorldTimerManager().ClearTimer(LevelTimerHandle);

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            AddScore(Score);
            CurrentLevelIndex++;
            SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;

            if (CurrentLevelIndex >= MaxLevels)
            {
                OnGameOver();
                return;
            }

            if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
            {
                UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
            }
            else
            {
                OnGameOver();
            }
        }
    }
}

void ASpartaGameState::OnGameOver()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            SpartaPlayerController->ShowMainMenu(true);
        }
    }
}

void ASpartaGameState::UpdateHUD()
{
    if (!GetWorld()) return;

    AsyncTask(ENamedThreads::GameThread, [this]()
        {
         if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
         {
            ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
            if (SpartaPlayerController)
                {
                    if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
                    {
                        if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
                        {
                            float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
                            TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
                        }

                        if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
                        {
                            if (UGameInstance* GameInstance = GetGameInstance())
                            {
                                USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
                                if (SpartaGameInstance)
                                {
                                    ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
                                }
                            }
                        }

                        if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
                        {
                            LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
                        }

                        if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
                        {
                            WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWave)));
                        }
                    }
                }
         }
    });
}

