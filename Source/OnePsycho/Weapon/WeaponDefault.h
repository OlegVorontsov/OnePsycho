// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/ArrowComponent.h"
#include "Types.h"
#include "ProjectileDefault.h"

#include "WeaponDefault.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadStart, UAnimMontage*, Anim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponReloadEnd);

UCLASS()
class ONEPSYCHO_API AWeaponDefault : public AActor
{
    GENERATED_BODY()

public:
    AWeaponDefault();

    FOnWeaponReloadStart OnWeaponReloadStart;
    FOnWeaponReloadEnd OnWeaponReloadEnd;

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
    //переменная для дополнительной структуры об оружии
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
    void ReloadTick(float DeltaTime);
    void DispersionTick(float DeltaTime);

    UFUNCTION(BlueprintCallable)
    void SetWeaponStateFire(bool bIsFire);

    bool CheckWeaponCanFire();

    FProjectileInfo GetProjectile();

    void Fire();

    FVector ApplyDispersionToShoot(FVector DirectionShoot) const;
    FVector GetFireEndLocation() const;

    void UpdateStateWeapon(EMovementState NewMovementState);
    void ChangeDispersionByShot();
    float GetCurrentDispersion() const;

    int8 GetNumberProjectileByShot() const;

    // Timers'flags
    float FireTimer = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
    float ReloadTimer = 0.0f;
    // debug
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic Debug")
    float ReloadTime = 0.0f;

    //функция возвращает кол-во выстрелов
    UFUNCTION(BlueprintCallable)
    int32 GetWeaponRound();

    void InitReload();
    void FinishReload();

    // flags
    bool BlockFire = false;
    // Dispersion
    bool ShouldReduceDispersion = false;
    float CurrentDispersion = 0.0f;
    float CurrentDispersionMax = 1.0f;
    float CurrentDispersionMin = 0.1f;
    float CurrentDispersionRecoil = 0.1f;
    float CurrentDispersionReduction = 0.1f;

    FVector ShootEndLocation = FVector(0);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool ShowDebug = false;
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    // bool byBarrel = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    float SizeVectorToChangeShootDirectionLogic = 100.0f;
};
