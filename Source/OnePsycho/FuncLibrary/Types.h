// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "Types.generated.h"

class AProjectileDefault;

//енам типов движения персонажа
UENUM(BlueprintType)
enum class EMovementState : uint8
{
    Aim_State UMETA(DisplayName = "Aim State"),
    AimWalk_State UMETA(DisplayName = "AimWalk State"),
    Walk_State UMETA(DisplayName = "Walk State"),
    Run_State UMETA(DisplayName = "Run State"),
    SprintRun_State UMETA(DisplayName = "SprintRun State")
};

//структура скоростей движения персонажа
USTRUCT(BlueprintType)
struct FCharacterSpeed
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AimSpeedNormal = 300.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float WalkSpeedNormal = 200.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float RunSpeedNormal = 600.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AimSpeedWalk = 100.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float SprintRunSpeedRun = 800.0f;
};

//структура о пуле
USTRUCT(BlueprintType)
struct FProjectileInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
    TSubclassOf<AProjectileDefault> Projectile = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
    float ProjectileDamage = 20.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
    float ProjectileLifeTime = 20.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
    float ProjectileInitSpeed = 2000.0f;

    // material to decal on hit
    /*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
    TMap<TEnumAsByte<EPhysicalSurface>, UMaterialInterface*> HitDecals;*/

    // Sound when hit
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
    // USoundBase* HitSound = nullptr;

    // fx when hit check by surface
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
    // TMap<TEnumAsByte<EPhysicalSurface>, UParticleSystem*> HitFXs;

    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
    // UParticleSystem* ExploseFX = nullptr;
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
    // USoundBase* ExploseSound = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
    bool bIsLikeBomp = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
    float ProjectileMaxRadiusDamage = 200.0f;
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
    // float ExploseMaxDamage = 40.0f;
    //  Timer add
};

//структура разброса при стрельбе в разных положениях персонажа
USTRUCT(BlueprintType)
struct FWeaponDispersion
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float DispersionAimStart = 0.5f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float DispersionAimMax = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float DispersionAimMin = 0.1f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float DispersionAimShootCoef = 1.0f;

    /*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float Aim_StateDispersionAimMax = 2.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float Aim_StateDispersionAimMin = 0.3f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float Aim_StateDispersionAimRecoil = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float Aim_StateDispersionReduction = .3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float AimWalk_StateDispersionAimMax = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float AimWalk_StateDispersionAimMin = 0.1f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float AimWalk_StateDispersionAimRecoil = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float AimWalk_StateDispersionReduction = 0.4f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float Walk_StateDispersionAimMax = 5.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float Walk_StateDispersionAimMin = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float Walk_StateDispersionAimRecoil = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float Walk_StateDispersionReduction = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float Run_StateDispersionAimMax = 10.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float Run_StateDispersionAimMin = 4.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float Run_StateDispersionAimRecoil = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    float Run_StateDispersionReduction = 0.1f;*/
};

//структура об оружии с доступом из таблиц
USTRUCT(BlueprintType)
struct FWeaponInfo : public FTableRowBase
{
    GENERATED_BODY()

    //класс оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    TSubclassOf<class AWeaponDefault> WeaponClass = nullptr;

    //частота выстрелов
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    float RateOfFire = 0.5f;

    //врем перезарядки
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    float ReloadTime = 2.0f;

    //мах кол-во патронов в магазине
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    int32 MaxRound = 10;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    int32 NumberProjectileByShot = 1;

    //добавляем структуру разброса при стрельбе
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
    FWeaponDispersion DispersionWeapon;

    //звуки
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound ")
    USoundBase* SoundFireWeapon = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound ")
    USoundBase* SoundReloadWeapon = nullptr;

    //эффект при выстреле
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX ")
    UParticleSystem* EffectFireWeapon = nullptr;

    //добавляем стрктуру о пуле
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile ")
    FProjectileInfo ProjectileSetting;

    // if null use trace logic (TSubclassOf<class AProjectileDefault> Projectile = nullptr)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace ")
    float WeaponDamage = 20.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace ")
    float DistacneTrace = 2000.0f;

    // one decal on all?
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitEffect ")
    UDecalComponent* DecalOnHit = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim ")
    UAnimMontage* AnimCharFire = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim ")
    UAnimMontage* AnimCharReload = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh ")
    UStaticMesh* MagazineDrop = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh ")
    UStaticMesh* ShellBullets = nullptr;
};

//дополнительная структура об оружии
USTRUCT(BlueprintType)
struct FAddicionalWeaponInfo
{
    GENERATED_BODY()

    //кол-во выстрелов
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    int32 Round = 10;
};

UCLASS()
class ONEPSYCHO_API UTypes : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
};
