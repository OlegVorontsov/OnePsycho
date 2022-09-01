
#pragma once

#include "CoreMinimal.h"
#include "OnePsychoHealthComponent.h"
#include "OnePsychoCharHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChange, float, Shield, float, Damage);

UCLASS()
class ONEPSYCHO_API UOnePsychoCharHealthComponent : public UOnePsychoHealthComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Shield")
    FOnShieldChange OnShieldChange;

    FTimerHandle TimerHandle_CoolDownShieldTimer;
    FTimerHandle TimerHandle_ShieldRecoveryRateTimer;

protected:
    float Shield = 100.0f;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
    float CoolDownShieldRecoverTime = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
    float ShieldRecoverValue = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
    float ShieldRecoverRate = 0.1f;

    void ChangeHealthValue(float ChangeValue) override;

    UFUNCTION(BlueprintCallable)
    float GetCurrentShield();

    void ChangeShieldValue(float ChangeValue);

    void CoolDownShieldEnd();

    void RecoveryShield();
};
