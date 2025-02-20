
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

ASpartaPlayerController::ASpartaPlayerController()
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	HUDWidgetClass(nullptr),
	HUDWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr),
	WaveText(nullptr)
{
}

void ASpartaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	SetPause(true);
	bShowMouseCursor = true;
	SetInputMode(FInputModeGameOnly());

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
	else
	{
		ShowGameHUD();
	}
}

UUserWidget* ASpartaPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}


void ASpartaPlayerController::ShowMainMenu(bool bIsRestart)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());


			if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
			{
				if (bIsRestart)
				{
					ButtonText->SetText(FText::FromString(TEXT("Restart")));
				}
				else
				{
					ButtonText->SetText(FText::FromString(TEXT("Start")));
				}
			}

			if (bIsRestart)
			{
				UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlatGameOverAnim"));
				if (PlayAnimFunc)
				{
					MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
				}

				if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("TotalScoreText")))
				{
					if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
					{
						TotalScoreText->SetText(FText::FromString(
							FString::Printf(TEXT("Total Score: %d"), SpartaGameInstance->TotalScore)
						));
					}
				}
			}

			if (bIsRestart)
			{
				SetPause(false);
			}
		}
	}
}


void ASpartaPlayerController::ShowGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			SetPause(false);
			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());

			ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
			if (SpartaGameState)
			{
				SpartaGameState->UpdateHUD();

				if (WaveText)
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), SpartaGameState->CurrentWave)));
				}
			}
		}
	}
}


void ASpartaPlayerController::StartGame()
{
	if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SpartaGameInstance->CurrentLevelIndex = 0;
		SpartaGameInstance->TotalScore = 0;
	}
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

		SetPause(false);
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
		
		ShowGameHUD();

	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
}


void ASpartaPlayerController::OnChangeLevelButtonClicked()
{
	if (ASpartaGameState* GameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr)
	{
		if (GameState->LevelMapNames.IsValidIndex(GameState->CurrentLevelIndex + 1))
		{
			GameState->CurrentLevelIndex++;
			FName NextLevel = GameState->LevelMapNames[GameState->CurrentLevelIndex];

			UGameplayStatics::OpenLevel(GetWorld(), NextLevel);

			FTimerHandle UpdateHUDTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				UpdateHUDTimerHandle,
				this,
				&ASpartaPlayerController::UpdateHUDAfterDelay,
				0.5f,
				false
			);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No more levels available!"));
		}
	}
}
void ASpartaPlayerController::UpdateHUDAfterDelay()
{
	if (ASpartaGameState* UpdatedGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr)
	{
		UpdatedGameState->UpdateHUD();
	}
}

void ASpartaPlayerController::OnEndGameButtonClicked()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("MenuLevel"));
}
