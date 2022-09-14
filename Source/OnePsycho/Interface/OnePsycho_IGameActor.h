
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StateEffect.h"
#include "OnePsycho_IGameActor.generated.h"

UINTERFACE(MinimalAPI)
class UOnePsycho_IGameActor : public UInterface
{
    GENERATED_BODY()
};

class ONEPSYCHO_API IOnePsycho_IGameActor
{
    GENERATED_BODY()

public:
    // UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Event")
    // void AviableForEffectsBP();

    // UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Event")
    // bool AviableForEffects();

    virtual EPhysicalSurface GetSurfaceType();

    virtual TArray<UStateEffect*> GetAllCurrentEffects();

    virtual void RemoveEffect(UStateEffect* RemoveEffect);

    virtual void AddEffect(UStateEffect* newEffect);
};
