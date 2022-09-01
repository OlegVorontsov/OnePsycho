
#include "OnePsychoHealthComponent.h"

UOnePsychoHealthComponent::UOnePsychoHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UOnePsychoHealthComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UOnePsychoHealthComponent::TickComponent(
    float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

float UOnePsychoHealthComponent::GetCurrentHealth()
{
    return Health;
}

void UOnePsychoHealthComponent::SetCurrentHealth(float NewHealth)
{
    Health = NewHealth;
}

void UOnePsychoHealthComponent::ChangeHealthValue(float ChangeValue)
{
    ChangeValue = ChangeValue * CoefDamage;

    Health += ChangeValue;

    if (Health > 100.0f)
    {
        Health = 100.0f;
    }
    else
    {
        if (Health < 0.0f)
        {
            OnDead.Broadcast();
        }
    }

    OnHealthChange.Broadcast(Health, ChangeValue);
}
