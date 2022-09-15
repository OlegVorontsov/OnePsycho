
#include "OnePsychoCharHealthComponent.h"

void UOnePsychoCharHealthComponent::ChangeHealthValue(float ChangeValue)
{
    float CurrentDamage = ChangeValue * CoefDamage;

    if (!bIsInvulnerable)
    {
        if (Shield > 0.0f && ChangeValue < 0.0f)
        {
            ChangeShieldValue(ChangeValue);
            if (Shield < 0.0f)
            {
                // fx
            }
        }
        else
        {
            Super::ChangeHealthValue(ChangeValue);
        }
    }
}

float UOnePsychoCharHealthComponent::GetCurrentShield()
{
    return Shield;
}

void UOnePsychoCharHealthComponent::ChangeShieldValue(float ChangeValue)
{
    Shield += ChangeValue;

    if (Shield > 100.0f)
    {
        Shield = 100.0f;
    }
    else
    {
        if (Shield < 0.0f)
            Shield = 0.0f;
    }

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(TimerHandle_CoolDownShieldTimer, this,
            &UOnePsychoCharHealthComponent::CoolDownShieldEnd, CoolDownShieldRecoverTime, false);

        GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldRecoveryRateTimer);
    }

    OnShieldChange.Broadcast(Shield, ChangeValue);
}

void UOnePsychoCharHealthComponent::CoolDownShieldEnd()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShieldRecoveryRateTimer, this,
            &UOnePsychoCharHealthComponent::RecoveryShield, ShieldRecoverRate, true);
    }
}

void UOnePsychoCharHealthComponent::RecoveryShield()
{
    float tmp = Shield;
    tmp = tmp + ShieldRecoverValue;
    if (tmp > 100.0f)
    {
        Shield = 100.0f;
        if (GetWorld())
        {
            GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldRecoveryRateTimer);
        }
    }
    else
        Shield = tmp;

    OnShieldChange.Broadcast(Shield, ShieldRecoverValue);
}

void UOnePsychoCharHealthComponent::SetInvulnerabilityEnabled()
{
    bIsInvulnerable = true;

    OnInvulnerabilityChange.Broadcast(bIsInvulnerable);
}

void UOnePsychoCharHealthComponent::SetInvulnerabilityDisabled()
{
    bIsInvulnerable = false;

    OnInvulnerabilityChange.Broadcast(bIsInvulnerable);
}
