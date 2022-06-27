// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldItemDefault.generated.h"

UCLASS()
class ONEPSYCHO_API AWorldItemDefault : public AActor
{
    GENERATED_BODY()

public:
    AWorldItemDefault();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};
