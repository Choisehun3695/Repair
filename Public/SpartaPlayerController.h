
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "SpartaPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UUserWidget;

UCLASS()
class REPAIR_API ASpartaPlayerController : public APlayerController
{
	GENERATED_BODY()

	public:
		ASpartaPlayerController();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		UInputMappingContext* InputMappingContext;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		UInputAction* MoveAction;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		UInputAction* JumpAction;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		UInputAction* LookAction;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		UInputAction* SprintAction;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		TSubclassOf<UUserWidget> HUDWidgetClass;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
		UUserWidget* HUDWidgetInstance;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
		TSubclassOf<UUserWidget> MainMenuWidgetClass;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu")
		UUserWidget* MainMenuWidgetInstance;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		UTextBlock* WaveText;


		UFUNCTION(BlueprintCallable, Category = "HUD")
		UUserWidget* GetHUDWidget() const;

		UFUNCTION(BlueprintCallable, Category = "HUD")
		void ShowGameHUD();


		UPROPERTY(meta = (Bindwidget))
		UButton* ChangeLevelButton;

		UFUNCTION(BlueprintCallable, Category = "HUD")
		void OnChangeLevelButtonClicked();

		UFUNCTION()
		void UpdateHUDAfterDelay();

		UFUNCTION(BlueprintCallable, Category = "HUD")
		void OnEndGameButtonClicked();

		UFUNCTION(BlueprintCallable, Category = "Menu")
		void ShowMainMenu(bool bIsRestart);
		UFUNCTION(BlueprintCallable, Category = "Menu")
		void StartGame();

	virtual void BeginPlay() override;
};
