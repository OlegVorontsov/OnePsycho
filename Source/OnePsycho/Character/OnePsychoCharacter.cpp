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
    CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

    // Create a camera...
    TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Create a decal in the world to show the cursor's location
    CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
    CursorToWorld->SetupAttachment(RootComponent);
    static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(
        TEXT("Material'/Game/Blueprint/Character/M_Cursor_Decal.M_Cursor_Decal'"));
    if (DecalMaterialAsset.Succeeded())
    {
        CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
    }
    CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
    CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

    // Activate ticking in order to update the cursor every frame.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void AOnePsychoCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (CursorToWorld != nullptr)
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
    }

    MovementTick(DeltaSeconds);
}

void AOnePsychoCharacter::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
    Super::SetupPlayerInputComponent(NewInputComponent);

    NewInputComponent->BindAxis(TEXT("MoveForward"), this, &AOnePsychoCharacter::InputAxisX);
    NewInputComponent->BindAxis(TEXT("MoveRight"), this, &AOnePsychoCharacter::InputAxisY);
}

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

    float ActualRotationYaw = GetActorRotation().Yaw;

    APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (myController && MovementState != EMovementState::SprintRun_State)
    {
        FHitResult ResultHit;
        myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);
        float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
        float YawDifference = FindRotatorResultYaw - ActualRotationYaw;

        // Коррекция определения ротации (в градусах)
        if (YawDifference < -180)
            YawDifference += 360;
        else if (YawDifference > 180)
            YawDifference -= 360;

        // Сглаживание изменения ротации
        if (-RotationChangeStep >= YawDifference)
            AddActorLocalRotation(FQuat(FRotator(0.0f, -RotationChangeStep, 0.0f)));
        else if (YawDifference >= RotationChangeStep)
            AddActorLocalRotation(FQuat(FRotator(0.0f, RotationChangeStep, 0.0f)));
        else
            SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
    }

    // Реализация выносливости
    if (MovementState == EMovementState::SprintRun_State && (AxisX != 0 || AxisY != 0))
    {
        if (SprintRunStamina > MovementSpeedInfo.RunSpeedNormal)
        {
            SprintRunStamina -= StaminaStepDown;
        }
        else
        {
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
    else
    {
        if (SprintRunStamina < SprintRunStaminaUpperLimit)
        {
            SprintRunStamina += StaminaStepUp;
        }
    }
}

// Изменение скорости передвижения
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

// Изменение режима передвижения
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
