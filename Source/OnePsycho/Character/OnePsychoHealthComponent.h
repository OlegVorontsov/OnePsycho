#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OnePsychoHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChange, float, Health, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDead);

USTRUCT(BlueprintType)
struct FStatsParam
{
    GENERATED_BODY()
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ONEPSYCHO_API UOnePsychoHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOnePsychoHealthComponent();

    UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Health")
    FOnHealthChange OnHealthChange;

    UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Health")
    FOnDead OnDead;

protected:
    virtual void BeginPlay() override;

    float Health = 100.0f;

public:
    virtual void TickComponent(
        float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Health")
    float GetCurrentHealth();

    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetCurrentHealth(float NewHealth);

    UFUNCTION(BlueprintCallable, Category = "Health")
    virtual void ChangeCurrentHealth(float ChangeValue);

    UFUNCTION(BlueprintNativeEvent)
    void DeadEvent();

    /*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
    float CoolDownShieldRecoverTime = 5.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
    float ShieldRecoverValue = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
    float ShieldRecoverRate = 0.1f;

    void ChangeHealthValue(float ChangeValue) override;

    float GetCurrentShield();

    void ChangeShieldValue(float ChangeValue);

    void CoolDownShieldEnd();

    void RecoveryShield();*/
};
