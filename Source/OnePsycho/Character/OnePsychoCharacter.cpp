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
#include "OnePsycho/Components/CharacterInventoryComponent.h"
#include "OnePsychoGameInstance.h"

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
    CameraBoom->TargetArmLength = 900.f;
    CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
    CameraBoom->bDoCollisionTest = true; // Don't want to pull camera in when it collides with level

    // Create a camera...
    TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // ??????? ??????? ?????????
    CharacterInventoryComponent = CreateDefaultSubobject<UCharacterInventoryComponent>(TEXT("InventoryComponent"));

    if (CharacterInventoryComponent)
    {
        CharacterInventoryComponent->OnSwitchWeapon.AddDynamic(this, &AOnePsychoCharacter::InitWeapon);
    }

    // Activate ticking in order to update the cursor every frame.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void AOnePsychoCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

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

    //????? ???????
    if (CursorMaterial)
    {
        CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
    }
}

UDecalComponent* AOnePsychoCharacter::GetCursorToWorld()
{
    return CurrentCursor;
}

//?????????????? ??????????? ??????? ??????? ??????
void AOnePsychoCharacter::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
    //??????????? ???????
    Super::SetupPlayerInputComponent(NewInputComponent);

    //?????????????? ??????? ?????? ?? ???? ???????
    NewInputComponent->BindAxis(TEXT("MoveForward"), this, &AOnePsychoCharacter::InputAxisX);
    NewInputComponent->BindAxis(TEXT("MoveRight"), this, &AOnePsychoCharacter::InputAxisY);

    NewInputComponent->BindAction(
        TEXT("FireEvent"), EInputEvent::IE_Pressed, this, &AOnePsychoCharacter::InputAttackPressed);
    NewInputComponent->BindAction(
        TEXT("FireEvent"), EInputEvent::IE_Released, this, &AOnePsychoCharacter::InputAttackReleased);
    NewInputComponent->BindAction(
        TEXT("ReloadEvent"), EInputEvent::IE_Released, this, &AOnePsychoCharacter::TryReloadWeapon);

    NewInputComponent->BindAction(
        TEXT("SwitchNextWeapon"), EInputEvent::IE_Pressed, this, &AOnePsychoCharacter::TrySwicthNextWeapon);
    NewInputComponent->BindAction(
        TEXT("SwitchPreviosWeapon"), EInputEvent::IE_Pressed, this, &AOnePsychoCharacter::TrySwitchPreviosWeapon);
}

//??????? ????????
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

    //???????? ????????? ?? ????????
    if (AxisX != 0 || AxisY != 0)
        CharMoving = true;
    else
        CharMoving = false;
    ChangeMovementState();

    //?????????? ? ?????????? ??????? ????????? ?????? ??? Z
    float ActualRotationYaw = GetActorRotation().Yaw;

    //?????????? ? ?????????? ?????????? ?????????
    APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (myController && MovementState != EMovementState::SprintRun_State)
    {
        //??????? ?????????? ??? ????? ???? ????????? ??????
        FHitResult ResultHit;

        //???????? ????? ??????????????? ??????? ?? ????????? ??????
        // myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);
        myController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);

        //???????? ???? ???????? ? ??????? ??????? ? ?????????? ? ??????????
        float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;

        //????????? ??????? ? ????? ???????? ? ??????? ??????? ? ?????????
        float YawDifference = FindRotatorResultYaw - ActualRotationYaw;

        //????????? ??????????? ??????? (? ????????)
        // if (YawDifference < -180)
        // YawDifference += 360;
        // else if (YawDifference > 180)
        // YawDifference -= 360;

        //??????????? ???????? ?????????
        // if (-RotationChangeStep >= YawDifference)
        // AddActorLocalRotation(FQuat(FRotator(0.0f, -RotationChangeStep, 0.0f)));
        // else if (YawDifference >= RotationChangeStep)
        // AddActorLocalRotation(FQuat(FRotator(0.0f, RotationChangeStep, 0.0f)));
        // else
        SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));

        //????????? ?????? ???????? ???????? ???????????? ???????
        if (CurrentWeapon)
        {
            //?????????? ?????? ?? ???? ?? ??????
            FVector Displacement = FVector(0);
            switch (MovementState)
            {
            case EMovementState::Aim_State:
                Displacement = FVector(0.0f, 0.0f, 160.0f);
                CurrentWeapon->ShouldReduceDispersion = true;
                break;
            case EMovementState::AimWalk_State:
                CurrentWeapon->ShouldReduceDispersion = true;
                Displacement = FVector(0.0f, 0.0f, 160.0f);
                break;
            case EMovementState::Walk_State:
                Displacement = FVector(0.0f, 0.0f, 120.0f);
                CurrentWeapon->ShouldReduceDispersion = false;
                break;
            case EMovementState::Run_State:
                Displacement = FVector(0.0f, 0.0f, 120.0f);
                CurrentWeapon->ShouldReduceDispersion = false;
                break;
            case EMovementState::SprintRun_State:
                break;
            default:
                break;
            }
            //???????? ? ?????? ????? ??????? + ?????? ??? ?????
            CurrentWeapon->ShootEndLocation = ResultHit.Location + Displacement;
            // aim cursor like 3d Widget?
        }
    }

    //?????????? ????????????

    //???? ??????? ?????? ? ???????? ?????????
    if (MovementState == EMovementState::SprintRun_State && (AxisX != 0 || AxisY != 0))
    {
        //???? ???????????? > ???????? ????
        if (SprintRunStamina > MovementSpeedInfo.RunSpeedNormal)
        {
            //????????? ????????????
            SprintRunStamina -= StaminaStepDown;
        }
        else
        {
            //????????? ?? ???
            SprintRunEnabled = false;
            MovementState = EMovementState::Run_State;
            CharacterUpdate();
        }
        //???? ???????????? < ???????? ???????
        /*if (SprintRunStamina < MovementSpeedInfo.SprintRunSpeedRun)
        {
            ResSpeed = SprintRunStamina;
            GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
        }*/
    }
    //??????????????? ????????????
    else
    {
        if (SprintRunStamina < SprintRunStaminaUpperLimit)
        {
            SprintRunStamina += StaminaStepUp;
        }
    }
}

// ????????? ???????? ????????
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

// ????????? ?????? ????????
void AOnePsychoCharacter::ChangeMovementState()
{
    if (SprintRunEnabled)
    {
        MovementState = EMovementState::SprintRun_State;
    }
    else
    {
        if (WalkEnabled && AimEnabled && CharMoving)
        {
            MovementState = EMovementState::AimWalk_State;
        }
        else if (WalkEnabled && !AimEnabled && CharMoving)
        {
            MovementState = EMovementState::Walk_State;
        }
        else if (!WalkEnabled && AimEnabled && CharMoving)
        {
            MovementState = EMovementState::Aim_State;
        }
        else if (!WalkEnabled && !AimEnabled && CharMoving)
        {
            MovementState = EMovementState::Run_State;
        }
        else if (!WalkEnabled && !AimEnabled && !CharMoving)
        {
            MovementState = EMovementState::Aim_State;
        }
    }
    CharacterUpdate();

    // Weapon state update
    AWeaponDefault* myWeapon = GetCurrentWeapon();
    if (myWeapon)
    {
        myWeapon->UpdateStateWeapon(MovementState);
    }
}

AWeaponDefault* AOnePsychoCharacter::GetCurrentWeapon()
{
    return CurrentWeapon;
}

//??????? ?????? ??????
void AOnePsychoCharacter::InitWeapon(
    FName IdWeapon, FAdditionalWeaponInfo WeaponAdditionalInfo) // int32 NewCurrentIndexWeapon)
{
    if (CurrentWeapon)
    {
        CurrentWeapon->Destroy();
        CurrentWeapon = nullptr;
    }

    UOnePsychoGameInstance* myGI = Cast<UOnePsychoGameInstance>(GetGameInstance());
    FWeaponInfo myWeaponInfo;
    if (myGI)
    {
        if (myGI->GetWeaponInfoByName(IdWeapon, myWeaponInfo))
        {
            if (myWeaponInfo.WeaponClass)
            {
                FVector SpawnLocation = FVector(0);
                FRotator SpawnRotation = FRotator(0);

                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                SpawnParams.Owner = this;
                SpawnParams.Instigator = GetInstigator();

                AWeaponDefault* myWeapon = Cast<AWeaponDefault>(
                    GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
                if (myWeapon)
                {
                    FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
                    myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
                    CurrentWeapon = myWeapon;

                    myWeapon->WeaponSetting = myWeaponInfo;
                    myWeapon->AdditionalWeaponInfo.Round = myWeaponInfo.MaxRound;

                    myWeapon->ReloadTime = myWeaponInfo.ReloadTime;
                    myWeapon->UpdateStateWeapon(MovementState);

                    myWeapon->AdditionalWeaponInfo = WeaponAdditionalInfo;
                    if (CharacterInventoryComponent)
                        // CurrentIndexWeapon = NewCurrentIndexWeapon;
                        CurrentIndexWeapon = CharacterInventoryComponent->GetWeaponIndexSlotByName(IdWeapon);

                    myWeapon->OnWeaponReloadStart.AddDynamic(this, &AOnePsychoCharacter::WeaponReloadStart);
                    myWeapon->OnWeaponReloadEnd.AddDynamic(this, &AOnePsychoCharacter::WeaponReloadEnd);
                    myWeapon->OnWeaponFireStart.AddDynamic(this, &AOnePsychoCharacter::WeaponFireStart);

                    // after swtch try reload if needed
                    if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
                        CurrentWeapon->InitReload();

                    if (CharacterInventoryComponent)
                        CharacterInventoryComponent->OnWeaponAmmoAviable.Broadcast(myWeapon->WeaponSetting.WeaponType);
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UOnePsychoGameInstance::InitWeapon - Weapon not found in table"));
        }
    }
}

//??????? ?? ????? ????????
void AOnePsychoCharacter::InputAttackPressed()
{
    AttackCharEvent(true);
}
void AOnePsychoCharacter::InputAttackReleased()
{
    AttackCharEvent(false);
}

//??????? ????????
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

void AOnePsychoCharacter::TryReloadWeapon()
{
    if (CurrentWeapon && !CurrentWeapon->WeaponReloading)
    {
        if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSetting.MaxRound &&
            CurrentWeapon->CheckCanWeaponReload())
        {
            CurrentWeapon->InitReload();
        }
    }
}

void AOnePsychoCharacter::WeaponFireStart(UAnimMontage* Anim)
{
    if (CharacterInventoryComponent && CurrentWeapon)
        CharacterInventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);

    WeaponFireStart_BP(Anim);
}

void AOnePsychoCharacter::WeaponReloadStart(UAnimMontage* Anim)
{
    WeaponReloadStart_BP(Anim);
}

void AOnePsychoCharacter::WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake)
{
    if (CharacterInventoryComponent && CurrentWeapon)
    {
        CharacterInventoryComponent->AmmoSlotChangeValue(CurrentWeapon->WeaponSetting.WeaponType, AmmoTake);
        CharacterInventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
    }
    WeaponReloadEnd_BP(bIsSuccess);
}

void AOnePsychoCharacter::WeaponFireStart_BP_Implementation(UAnimMontage* Anim) {}

void AOnePsychoCharacter::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim) {}

void AOnePsychoCharacter::WeaponReloadEnd_BP_Implementation(bool bIsSuccess) {}

void AOnePsychoCharacter::TrySwicthNextWeapon()
{
    // We have more then one weapon go switch
    if (CharacterInventoryComponent->WeaponSlots.Num() > 1)
    {
        int8 OldIndex = CurrentIndexWeapon;
        FAdditionalWeaponInfo OldInfo;
        if (CurrentWeapon)
        {
            //?????????? ???? ???????? ??????
            OldInfo = CurrentWeapon->AdditionalWeaponInfo;
            if (CurrentWeapon->WeaponReloading)
                CurrentWeapon->CancelReload();
        }

        if (CharacterInventoryComponent)
        {
            if (CharacterInventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon + 1, OldIndex, OldInfo, true))
            {
            }
        }
    }
}

void AOnePsychoCharacter::TrySwitchPreviosWeapon()
{
    // We have more then one weapon go switch
    if (CharacterInventoryComponent->WeaponSlots.Num() > 1)
    {
        int8 OldIndex = CurrentIndexWeapon;
        FAdditionalWeaponInfo OldInfo;
        if (CurrentWeapon)
        {
            //?????????? ???? ???????? ??????
            OldInfo = CurrentWeapon->AdditionalWeaponInfo;
            if (CurrentWeapon->WeaponReloading)
                CurrentWeapon->CancelReload();
        }

        if (CharacterInventoryComponent)
        {
            // InventoryComponent->SetAdditionalInfoWeapon(OldIndex, GetCurrentWeapon()->AdditionalWeaponInfo);
            if (CharacterInventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon - 1, OldIndex, OldInfo, false))
            {
            }
        }
    }
}
