// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnePsychoGameMode.h"
#include "OnePsychoPlayerController.h"
#include "OnePsycho/Character/OnePsychoCharacter.h"
#include "UObject/ConstructorHelpers.h"

AOnePsychoGameMode::AOnePsychoGameMode()
{
    // use our custom PlayerController class
    PlayerControllerClass = AOnePsychoPlayerController::StaticClass();

    // set default pawn class to our Blueprinted character
    /*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/Character/BP_Character"));
    if (PlayerPawnBPClass.Class != nullptr)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }*/
    DefaultPawnClass = AOnePsychoCharacter::StaticClass();
}

void AOnePsychoGameMode::PlayerCharacterDead() {}
