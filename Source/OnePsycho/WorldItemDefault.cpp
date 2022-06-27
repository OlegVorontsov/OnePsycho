// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldItemDefault.h"

AWorldItemDefault::AWorldItemDefault()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AWorldItemDefault::BeginPlay()
{
    Super::BeginPlay();
}

void AWorldItemDefault::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
