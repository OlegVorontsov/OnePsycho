
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

void UOnePsychoHealthComponent::ChangeCurrentHealth(float ChangeValue)
{
    Health += ChangeValue;
    OnHealthChange.Broadcast(Health, ChangeValue);
    if (Health < 0.0f)
    {
        OnDead.Broadcast();
        DeadEvent();
    }
}

void UOnePsychoHealthComponent::DeadEvent_Implementation() {}
