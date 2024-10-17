// Copyright Epic Games, Inc. All Rights Reserved.

#include "MutipleLogTestGameMode.h"
#include "MutipleLogTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMutipleLogTestGameMode::AMutipleLogTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
