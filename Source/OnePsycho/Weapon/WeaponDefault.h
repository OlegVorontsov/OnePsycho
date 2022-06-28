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

    //объекты для создания оружия
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
    class USceneComponent* SceneComponent = nullptr;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
    class USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
    class UStaticMeshComponent* StaticMeshWeapon = nullptr;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
    class UArrowComponent* ShootLocation = nullptr;

    //переменная для структуры об оружии
    UPROPERTY()
    FWeaponInfo WeaponSetting;
    //??????
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
    FAddicionalWeaponInfo WeaponInfo;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    void WeaponInit();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
    bool WeaponFiring = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
    bool WeaponReloading = false;

    void FireTick(float DeltaTime);

    UFUNCTION(BlueprintCallable)
    void SetWeaponStateFire(bool bIsFire);

    bool CheckWeaponCanFire();

    FProjectileInfo GetProjectile();

    void Fire();

    void UpdateStateWeapon(EMovementState NewMovementState);
    void ChangeDispersion();

    // Timers'flags
    float FireTime = 0.0;
};
