// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/ArrowComponent.h"
#include "Types.h"
#include "ProjectileDefault.h"

#include "WeaponDefault.generated.h"

// DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFireStart);//ToDo Delegate on event weapon fire - Anim char, state
// char...

UCLASS()
class ONEPSYCHO_API AWeaponDefault : public AActor
{
    GENERATED_BODY()

public:
    AWeaponDefault();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
    class USceneComponent* SceneComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
    class USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
    class UStaticMeshComponent* StaticMeshWeapon = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
    class UArrowComponent* ShootLocation = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
    FWeaponInfo WeaponSetting;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    void WeaponInit();

    void FireTick(float DeltaTime);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
    bool WeaponFiring = false;

    UFUNCTION(BlueprintCallable)
    void SetWeaponStateFire(bool bIsFire);

    bool CheckWeaponCanFire();

    FProjectileInfo GetProjectile();

    void Fire();

    // void UpdateStateWeapon(EMovementState NewMovementState);
    // void ChangeDispersion();

    // Timers'flags
    float FireTime = 0.0;
};
