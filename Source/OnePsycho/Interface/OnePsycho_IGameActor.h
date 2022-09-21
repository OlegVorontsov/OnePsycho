
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StateEffect.h"
#include "Types.h"
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

    // inventory
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void DropWeaponToWorld(FDropItem DropItemInfo);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void DropAmmoToWorld(EWeaponType TypeAmmo, int32 Cout);
};
