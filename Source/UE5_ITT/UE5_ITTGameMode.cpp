// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5_ITTGameMode.h"
#include "UE5_ITTPlayerController.h"
#include "UE5_ITTCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUE5_ITTGameMode::AUE5_ITTGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AUE5_ITTPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}