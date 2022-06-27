// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponDefault.h"

AWeaponDefault::AWeaponDefault()
{
    PrimaryActorTick.bCanEverTick = true;

    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    RootComponent = SceneComponent;

    SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
    SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
    SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
    SkeletalMeshWeapon->SetupAttachment(RootComponent);

    StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
    StaticMeshWeapon->SetGenerateOverlapEvents(false);
    StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
    StaticMeshWeapon->SetupAttachment(RootComponent);

    ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootLocation"));
    ShootLocation->SetupAttachment(RootComponent);
}

void AWeaponDefault::BeginPlay()
{
    Super::BeginPlay();

    WeaponInit();
}

void AWeaponDefault::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FireTick(DeltaTime);
}

void AWeaponDefault::WeaponInit()
{
    if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
    {
        SkeletalMeshWeapon->DestroyComponent(true);
    }

    if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
    {
        StaticMeshWeapon->DestroyComponent();
    }
}

void AWeaponDefault::SetWeaponStateFire(bool bIsFire)
{
    if (CheckWeaponCanFire())
        WeaponFiring = bIsFire;
    else
        WeaponFiring = false;
}

bool AWeaponDefault::CheckWeaponCanFire()
{
    return true;
}

void AWeaponDefault::FireTick(float DeltaTime)
{
    if (WeaponFiring)
        if (FireTime < 0.f)
            Fire();
        else
            FireTime -= DeltaTime;
}

void AWeaponDefault::Fire()
{
    FireTime = WeaponSetting.RateOfFire;

    if (ShootLocation)
    {
        FVector SpawnLocation = ShootLocation->GetComponentLocation();
        FRotator SpawnRotation = ShootLocation->GetComponentRotation();
        FProjectileInfo ProjectileInfo;
        ProjectileInfo = GetProjectile();

        if (ProjectileInfo.Projectile)
        {
            // Projectile Init ballistic fire

            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            SpawnParams.Owner = GetOwner();
            SpawnParams.Instigator = GetInstigator();

            AProjectileDefault* myProjectile = Cast<AProjectileDefault>(
                GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
            if (myProjectile)
            {
                // ToDo Init Projectile settings by id in table row(or keep in weapon table)
                myProjectile->InitialLifeSpan = 20.0f;
                // Projectile->BulletProjectileMovement->InitialSpeed = 2500.0f;
            }
        }
        else
        {
            // ToDo Projectile null Init trace fire
        }
    }
}

FProjectileInfo AWeaponDefault::GetProjectile()
{
    return WeaponSetting.ProjectileSettings;
}
