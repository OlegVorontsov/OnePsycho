
#include "StateEffect.h"
#include "OnePsychoHealthComponent.h"
#include "OnePsychoCharHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "OnePsycho_IGameActor.h"

bool UStateEffect::InitObject(AActor* Actor)
{
    myActor = Actor;

    IOnePsycho_IGameActor* myInterface = Cast<IOnePsycho_IGameActor>(myActor);
    if (myInterface)
    {
        myInterface->AddEffect(this);
    }

    return true;
}

void UStateEffect::DestroyObject()
{
    IOnePsycho_IGameActor* myInterface = Cast<IOnePsycho_IGameActor>(myActor);
    if (myInterface)
    {
        myInterface->RemoveEffect(this);
    }

    myActor = nullptr;
    if (this && this->IsValidLowLevel())
    {
        this->ConditionalBeginDestroy();
    }
}

bool UStateEffect_ExecuteOnce::InitObject(AActor* Actor)
{
    // return Super::InitObject(Actor);

    Super::InitObject(Actor);
    ExecuteOnce();
    return true;
}

void UStateEffect_ExecuteOnce::ExecuteOnce()
{
    if (myActor)
    {
        UOnePsychoHealthComponent* myHealthComp =
            Cast<UOnePsychoHealthComponent>(myActor->GetComponentByClass(UOnePsychoHealthComponent::StaticClass()));

        if (myHealthComp)
        {
            myHealthComp->ChangeHealthValue(Power);
        }
    }
    DestroyObject();
}

void UStateEffect_ExecuteOnce::DestroyObject()
{
    Super::DestroyObject();
}

bool UStateEffect_ExecuteTimer::InitObject(AActor* Actor)
{
    Super::InitObject(Actor);

    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle_EffectTimer, this, &UStateEffect_ExecuteTimer::DestroyObject, Timer, false);
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle_ExecuteTimer, this, &UStateEffect_ExecuteTimer::Execute, RateTime, true);

    if (ParticleEffect)
    {
        FName NameBoneToAttached;
        FVector Loc = FVector(0);

        ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect, //
            myActor->GetRootComponent(),                                         //
            NameBoneToAttached,                                                  //
            Loc,                                                                 //
            FRotator::ZeroRotator,                                               //
            EAttachLocation::SnapToTarget,                                       //
            false);
    }
    return true;
}

void UStateEffect_ExecuteTimer::DestroyObject()
{
    ParticleEmitter->DestroyComponent();
    ParticleEmitter = nullptr;

    Super::DestroyObject();
}

void UStateEffect_ExecuteTimer::Execute()
{
    if (myActor)
    {
        UOnePsychoHealthComponent* myHealthComp =
            Cast<UOnePsychoHealthComponent>(myActor->GetComponentByClass(UOnePsychoHealthComponent::StaticClass()));

        if (myHealthComp)
        {
            myHealthComp->ChangeHealthValue(Power);
        }
    }
}

bool UStateEffect_InvulnerabilityTimer::InitObject(AActor* Actor)
{
    Super::InitObject(Actor);
    Execute();

    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle_EffectTimer, this, &UStateEffect_InvulnerabilityTimer::DestroyObject, Timer, false);

    return true;
}

void UStateEffect_InvulnerabilityTimer::DestroyObject()
{
    if (myActor)
    {
        UOnePsychoCharHealthComponent* myHealthComp = Cast<UOnePsychoCharHealthComponent>(
            myActor->GetComponentByClass(UOnePsychoCharHealthComponent::StaticClass()));

        if (myHealthComp)
        {
            myHealthComp->SetInvulnerabilityDisabled();
        }
    }

    Super::DestroyObject();
}

void UStateEffect_InvulnerabilityTimer::Execute()
{
    if (myActor)
    {
        UOnePsychoCharHealthComponent* myHealthComp = Cast<UOnePsychoCharHealthComponent>(
            myActor->GetComponentByClass(UOnePsychoCharHealthComponent::StaticClass()));

        if (myHealthComp)
        {
            myHealthComp->SetInvulnerabilityEnabled();
        }
    }
}
