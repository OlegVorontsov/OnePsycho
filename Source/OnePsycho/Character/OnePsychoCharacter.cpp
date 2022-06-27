// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnePsychoCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "OnePsycho/Weapon/WeaponDefault.h"

AOnePsychoCharacter::AOnePsychoCharacter()
{
    // Set size for player capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate character to camera direction
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
    GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;

    // Create a camera boom...
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
    CameraBoom->TargetArmLength = 800.f;
    CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
    CameraBoom->bDoCollisionTest = true; // Don't want to pull camera in when it collides with level

    // Create a camera...
    TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Create a decal in the world to show the cursor's location
    /*CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
    CursorToWorld->SetupAttachment(RootComponent);
    static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(
        TEXT("Material'/Game/Blueprint/Character/M_Cursor_Decal.M_Cursor_Decal'"));
    if (DecalMaterialAsset.Succeeded())
    {
        CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
    }
    CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
    CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());*/

    // Activate ticking in order to update the cursor every frame.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void AOnePsychoCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    /*if (CursorToWorld != nullptr)
    {
        if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
        {
            if (UWorld* World = GetWorld())
            {
                FHitResult HitResult;
                FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
                FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
                FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
                Params.AddIgnoredActor(this);
                World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
                FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
                CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
            }
        }
        else if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            FHitResult TraceHitResult;
            PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
            FVector CursorFV = TraceHitResult.ImpactNormal;
            FRotator CursorR = CursorFV.Rotation();
            CursorToWorld->SetWorldLocation(TraceHitResult.Location);
            CursorToWorld->SetWorldRotation(CursorR);
        }
    }*/
    if (CurrentCursor)
    {
        APlayerController* myPC = Cast<APlayerController>(GetController());
        if (myPC)
        {
            FHitResult TraceHitResult;
            myPC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
            FVector CursorFV = TraceHitResult.ImpactNormal;
            FRotator CursorR = CursorFV.Rotation();

            CurrentCursor->SetWorldLocation(TraceHitResult.Location);
            CurrentCursor->SetWorldRotation(CursorR);
        }
    }
    MovementTick(DeltaSeconds);
}

void AOnePsychoCharacter::BeginPlay()
{
    Super::BeginPlay();

    //спавн оружия
    InitWeapon();

    //спавн курсора
    if (CursorMaterial)
    {
        CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
    }
}

UDecalComponent* AOnePsychoCharacter::GetCursorToWorld()
{
    return CurrentCursor;
}

//переопределяем стандартную функцию нажатия клавиш
void AOnePsychoCharacter::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
    //стандартная функция
    Super::SetupPlayerInputComponent(NewInputComponent);

    //переопределяем нажатия клавиш на наши функции
    NewInputComponent->BindAxis(TEXT("MoveForward"), this, &AOnePsychoCharacter::InputAxisX);
    NewInputComponent->BindAxis(TEXT("MoveRight"), this, &AOnePsychoCharacter::InputAxisY);

    NewInputComponent->BindAction(
        TEXT("FireEvent"), EInputEvent::IE_Pressed, this, &AOnePsychoCharacter::InputAttackPressed);
    NewInputComponent->BindAction(
        TEXT("FireEvent"), EInputEvent::IE_Released, this, &AOnePsychoCharacter::InputAttackReleased);
}

//функции движения
void AOnePsychoCharacter::InputAxisX(float Value)
{
    AxisX = Value;
}
void AOnePsychoCharacter::InputAxisY(float Value)
{
    AxisY = Value;
}
void AOnePsychoCharacter::MovementTick(float DeltaTime)
{
    AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisX);
    AddMovementInput(FVector(0.0f, 1.0f, 0.0f), AxisY);

    //записываем в переменную поворот персонажа вокруг оси Z
    float ActualRotationYaw = GetActorRotation().Yaw;

    //записываем в переменную контроллер персонажа
    APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (myController && MovementState != EMovementState::SprintRun_State)
    {
        //создаем переменную для точки куда указывает курсор
        FHitResult ResultHit;

        //получаем точки соприкосновения курсора по созданноу каналу
        // myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);
        myController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);

        //получаем угол поворота в сторону курсора и записываем в переменную
        float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;

        //вычисляем разницу в углах поворота в сторону курсора и персонажа
        float YawDifference = FindRotatorResultYaw - ActualRotationYaw;

        //коррекция определения ротации (в градусах)
        if (YawDifference < -180)
            YawDifference += 360;
        else if (YawDifference > 180)
            YawDifference -= 360;

        //сглаживание поворота персонажа
        if (-RotationChangeStep >= YawDifference)
            AddActorLocalRotation(FQuat(FRotator(0.0f, -RotationChangeStep, 0.0f)));
        else if (YawDifference >= RotationChangeStep)
            AddActorLocalRotation(FQuat(FRotator(0.0f, RotationChangeStep, 0.0f)));
        else
            SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
    }

    //реализация выносливости

    //если включен спринт и персонжа двигается
    if (MovementState == EMovementState::SprintRun_State && (AxisX != 0 || AxisY != 0))
    {
        //если выносливость > скорости бега
        if (SprintRunStamina > MovementSpeedInfo.RunSpeedNormal)
        {
            //уменьшаем выносливость
            SprintRunStamina -= StaminaStepDown;
        }
        else
        {
            //переходим на бег
            SprintRunEnabled = false;
            MovementState = EMovementState::Run_State;
            CharacterUpdate();
        }

        if (SprintRunStamina < MovementSpeedInfo.SprintRunSpeedRun)
        {
            ResSpeed = SprintRunStamina;
            GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
        }
    }
    //восстанавливаем выносливость
    else
    {
        if (SprintRunStamina < SprintRunStaminaUpperLimit)
        {
            SprintRunStamina += StaminaStepUp;
        }
    }
}

// Изменение скорости движения
void AOnePsychoCharacter::CharacterUpdate()
{
    switch (MovementState)
    {
    case EMovementState::Aim_State:
        ResSpeed = MovementSpeedInfo.AimSpeedNormal;
        break;
    case EMovementState::AimWalk_State:
        ResSpeed = MovementSpeedInfo.AimSpeedWalk;
        break;
    case EMovementState::Walk_State:
        ResSpeed = MovementSpeedInfo.WalkSpeedNormal;
        break;
    case EMovementState::Run_State:
        ResSpeed = MovementSpeedInfo.RunSpeedNormal;
        break;

    case EMovementState::SprintRun_State:
        if (SprintRunStamina < MovementSpeedInfo.SprintRunSpeedRun)
            ResSpeed = SprintRunStamina;
        else
            ResSpeed = MovementSpeedInfo.SprintRunSpeedRun;
        break;

    default:
        break;
    }
    GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

// Изменение режима движения
void AOnePsychoCharacter::ChangeMovementState()
{
    if (SprintRunEnabled)
    {
        MovementState = EMovementState::SprintRun_State;
    }
    else
    {
        if (WalkEnabled && AimEnabled)
        {
            MovementState = EMovementState::AimWalk_State;
        }
        else if (WalkEnabled && !AimEnabled)
        {
            MovementState = EMovementState::Walk_State;
        }
        else if (!WalkEnabled && AimEnabled)
        {
            MovementState = EMovementState::Aim_State;
        }
        else if (!WalkEnabled && !AimEnabled)
        {
            MovementState = EMovementState::Run_State;
        }
    }
    CharacterUpdate();
}

AWeaponDefault* AOnePsychoCharacter::GetCurrentWeapon()
{
    return CurrentWeapon;
}

//спавн оружия
void AOnePsychoCharacter::InitWeapon()
{
    if (InitWeaponClass)
    {
        FVector SpawnLocation = FVector(0);
        FRotator SpawnRotation = FRotator(0);

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnParams.Owner = GetOwner();
        SpawnParams.Instigator = GetInstigator();

        AWeaponDefault* myWeapon =
            Cast<AWeaponDefault>(GetWorld()->SpawnActor(InitWeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
        if (myWeapon)
        {
            FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
            myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
            CurrentWeapon = myWeapon;

            // myWeapon->UpdateStateWeapon(MovementState);
        }
    }
}

//функции на инпут стрельбы
void AOnePsychoCharacter::InputAttackPressed()
{
    AttackCharEvent(true);
}
void AOnePsychoCharacter::InputAttackReleased()
{
    AttackCharEvent(false);
}

//функция стрельбы
void AOnePsychoCharacter::AttackCharEvent(bool bIsFiring)
{
    AWeaponDefault* myWeapon = nullptr;
    myWeapon = GetCurrentWeapon();
    if (myWeapon)
    {
        // ToDo Check melee or range
        myWeapon->SetWeaponStateFire(bIsFiring);
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("AOnePsychoCharacter::AttackCharEvent - CurrentWeapon - NULL"));
}
