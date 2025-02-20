
#include "SpartaGameMode.h"
#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"

ASpartaGameMode::ASpartaGameMode() {
	DefaultPawnClass = ASpartaGameMode::StaticClass();
	PlayerControllerClass = ASpartaPlayerController::StaticClass();
	GameStateClass = ASpartaGameState::StaticClass();
}