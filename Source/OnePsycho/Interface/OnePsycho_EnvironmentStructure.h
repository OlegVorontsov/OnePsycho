
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OnePsycho_IGameActor.h"
#include "StateEffect.h"
#include "OnePsycho_EnvironmentStructure.generated.h"

UCLASS()
class ONEPSYCHO_API AOnePsycho_EnvironmentStructure : public AActor, public IOnePsycho_IGameActor
{
    GENERATED_BODY()

public:
    AOnePsycho_EnvironmentStructure();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    EPhysicalSurface GetSurfaceType() override;

    TArray<UStateEffect*> GetAllCurrentEffects() override;

    void RemoveEffect(UStateEffect* RemoveEffect) override;

    void AddEffect(UStateEffect* newEffect) override;

    // Effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    TArray<UStateEffect*> Effects;
};
