// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OnePsychoGameMode.generated.h"

UCLASS(minimalapi)
class AOnePsychoGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AOnePsychoGameMode();

    void PlayerCharacterDead();
};
