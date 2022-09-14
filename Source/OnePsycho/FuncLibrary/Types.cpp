// Fill out your copyright notice in the Description page of Project Settings.

#include "Types.h"
#include "OnePsycho/OnePsycho.h"
#include "OnePsycho_IGameActor.h"

void UTypes::AddEffectBySurfaceType(
    AActor* TakeEffectActor, TSubclassOf<UStateEffect> AddEffectClass, EPhysicalSurface SurfaceType)
{
    if (SurfaceType != EPhysicalSurface::SurfaceType_Default && TakeEffectActor && AddEffectClass)
    {
        UStateEffect* myEffect = Cast<UStateEffect>(AddEffectClass->GetDefaultObject());
        if (myEffect)
        {
            bool bIsHavePossibleSurface = false;
            int8 i = 0;
            while (i < myEffect->PossibleInteractSurface.Num() && !bIsHavePossibleSurface)
            {
                if (myEffect->PossibleInteractSurface[i] == SurfaceType)
                {
                    bIsHavePossibleSurface = true;
                    bool bIsCanAddEffect = false;
                    if (!myEffect->bIsStakable)
                    {
                        int8 j = 0;
                        TArray<UStateEffect*> CurrentEffects;
                        IOnePsycho_IGameActor* myInterface = Cast<IOnePsycho_IGameActor>(TakeEffectActor);
                        if (myInterface)
                        {
                            CurrentEffects = myInterface->GetAllCurrentEffects();
                        }
                        if (CurrentEffects.Num() > 0)
                        {
                            while (j < CurrentEffects.Num() && !bIsCanAddEffect)
                            {
                                if (CurrentEffects[i]->GetClass() != AddEffectClass)
                                {
                                    bIsCanAddEffect = true;
                                }
                                j++;
                            }
                        }
                        else
                        {
                            bIsCanAddEffect = true;
                        }
                    }
                    else
                    {
                        bIsCanAddEffect = true;
                    }

                    if (bIsCanAddEffect)
                    {
                        UStateEffect* NewEffect = NewObject<UStateEffect>(TakeEffectActor, AddEffectClass);
                        if (NewEffect)
                        {
                            NewEffect->InitObject(TakeEffectActor);
                        }
                    }
                }
                i++;
            }
        }
    }
}
