// Copyright Epic Games, Inc. All Rights Reserved.

#include "CosmophobiaGameMode.h"
#include "CosmophobiaCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACosmophobiaGameMode::ACosmophobiaGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
