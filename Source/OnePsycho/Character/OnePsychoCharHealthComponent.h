
#pragma once

#include "CoreMinimal.h"
#include "OnePsychoHealthComponent.h"
#include "OnePsychoCharHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChange, float, Shield, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvulnerabilityChange, bool, bIsInvulnerable);

UCLASS()
class ONEPSYCHO_API UOnePsychoCharHealthComponent : public UOnePsychoHealthComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Shield")
    FOnShieldChange OnShieldChange;

    UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Invulnerability")
    FOnInvulnerabilityChange OnInvulnerabilityChange;

    FTimerHandle TimerHandle_CoolDownShieldTimer;
    FTimerHandle TimerHandle_ShieldRecoveryRateTimer;

protected:
    float Shield = 100.0f;

    bool bIsInvulnerable = false;

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

    void SetInvulnerabilityEnabled();
    void SetInvulnerabilityDisabled();

    UFUNCTION(BlueprintCallable)
    float GetShieldValue();
};
