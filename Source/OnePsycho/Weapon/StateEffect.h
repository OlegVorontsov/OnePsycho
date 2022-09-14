
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystemComponent.h"
#include "StateEffect.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ONEPSYCHO_API UStateEffect : public UObject
{
    GENERATED_BODY()

public:
    virtual bool InitObject(AActor* Actor);

    virtual void DestroyObject();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
    TArray<TEnumAsByte<EPhysicalSurface>> PossibleInteractSurface;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
    bool bIsStakable = false;

    AActor* myActor = nullptr;
};

//разовый эффект
UCLASS()
class ONEPSYCHO_API UStateEffect_ExecuteOnce : public UStateEffect
{
    GENERATED_BODY()

public:
    bool InitObject(AActor* Actor) override;
    void DestroyObject() override;

    virtual void ExecuteOnce();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Once")
    float Power = 20.0f;
};

//эффект по таймеру
UCLASS()
class ONEPSYCHO_API UStateEffect_ExecuteTimer : public UStateEffect
{
    GENERATED_BODY()

public:
    bool InitObject(AActor* Actor) override;
    void DestroyObject() override;

    virtual void Execute();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting ExecuteTimer")
    float Power = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting ExecuteTimer")
    float Timer = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting ExecuteTimer")
    float RateTime = 1.0f;

    FTimerHandle TimerHandle_ExecuteTimer;
    FTimerHandle TimerHandle_EffectTimer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting ExecuteTimer")
    UParticleSystem* ParticleEffect = nullptr;

    UParticleSystemComponent* ParticleEmitter = nullptr;
};
