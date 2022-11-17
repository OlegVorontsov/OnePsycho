
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
#include "ProjectileDefault.h"

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

    // Создаем систему инвенторя
    CharacterInventoryComponent = CreateDefaultSubobject<UCharacterInventoryComponent>(TEXT("InventoryComponent"));

    // Создаем систему здоровья
    CharHealthComponent = CreateDefaultSubobject<UOnePsychoCharHealthComponent>(TEXT("HealthComponent"));

    if (CharHealthComponent)
    {
        CharHealthComponent->OnDead.AddDynamic(this, &AOnePsychoCharacter::CharDead);
    }

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

    // спавн курсора
    if (CursorMaterial)
    {
        CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
    }
}

UDecalComponent* AOnePsychoCharacter::GetCursorToWorld()
{
    return CurrentCursor;
}

// переопределяем стандартную функцию нажатия клавиш
void AOnePsychoCharacter::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
    // стандартная функция
    Super::SetupPlayerInputComponent(NewInputComponent);

    // переопределяем нажатия клавиш на наши функции
    NewInputComponent->BindAxis(TEXT("MoveForward"), this, &AOnePsychoCharacter::InputAxisX);
    NewInputComponent->BindAxis(TEXT("MoveRight"), this, &AOnePsychoCharacter::InputAxisY);

    NewInputComponent->BindAction(
        TEXT("FireEvent"), EInputEvent::IE_Pressed, this, &AOnePsychoCharacter::InputAttackPressed);
    NewInputComponent->BindAction(
        TEXT("FireEvent"), EInputEvent::IE_Released, this, &AOnePsychoCharacter::InputAttackReleased);
    NewInputComponent->BindAction(
        TEXT("ReloadEvent"), EInputEvent::IE_Released, this, &AOnePsychoCharacter::TryReloadWeapon);

    NewInputComponent->BindAction(
        TEXT("SwitchNextWeapon"), EInputEvent::IE_Pressed, this, &AOnePsychoCharacter::TrySwitchNextWeapon);
    NewInputComponent->BindAction(
        TEXT("SwitchPreviosWeapon"), EInputEvent::IE_Pressed, this, &AOnePsychoCharacter::TrySwitchPreviosWeapon);

    NewInputComponent->BindAction(
        TEXT("AbilityAction"), EInputEvent::IE_Pressed, this, &AOnePsychoCharacter::TryAbilityEnabled);

    NewInputComponent->BindAction(
        TEXT("DropCurrentWeapon"), EInputEvent::IE_Pressed, this, &AOnePsychoCharacter::DropCurrentWeapon);

    TArray<FKey> HotKeys;
    HotKeys.Add(EKeys::One);
    HotKeys.Add(EKeys::Two);
    HotKeys.Add(EKeys::Three);
    HotKeys.Add(EKeys::Four);
    HotKeys.Add(EKeys::Five);
    HotKeys.Add(EKeys::Six);
    HotKeys.Add(EKeys::Seven);
    HotKeys.Add(EKeys::Eight);
    HotKeys.Add(EKeys::Nine);
    HotKeys.Add(EKeys::Zero);

    NewInputComponent->BindKey(HotKeys[1], IE_Pressed, this, &AOnePsychoCharacter::TKeyPressed<1>);
    NewInputComponent->BindKey(HotKeys[2], IE_Pressed, this, &AOnePsychoCharacter::TKeyPressed<2>);
    NewInputComponent->BindKey(HotKeys[3], IE_Pressed, this, &AOnePsychoCharacter::TKeyPressed<3>);
    NewInputComponent->BindKey(HotKeys[4], IE_Pressed, this, &AOnePsychoCharacter::TKeyPressed<4>);
    NewInputComponent->BindKey(HotKeys[5], IE_Pressed, this, &AOnePsychoCharacter::TKeyPressed<5>);
    NewInputComponent->BindKey(HotKeys[6], IE_Pressed, this, &AOnePsychoCharacter::TKeyPressed<6>);
    NewInputComponent->BindKey(HotKeys[7], IE_Pressed, this, &AOnePsychoCharacter::TKeyPressed<7>);
    NewInputComponent->BindKey(HotKeys[8], IE_Pressed, this, &AOnePsychoCharacter::TKeyPressed<8>);
    NewInputComponent->BindKey(HotKeys[9], IE_Pressed, this, &AOnePsychoCharacter::TKeyPressed<9>);
    NewInputComponent->BindKey(HotKeys[0], IE_Pressed, this, &AOnePsychoCharacter::TKeyPressed<0>);
}

// функции движения
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
    if (bIsAlive)
    {
        AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisX);
        AddMovementInput(FVector(0.0f, 1.0f, 0.0f), AxisY);

        // проверка двигается ли персонаж
        if (AxisX != 0 || AxisY != 0)
            CharMoving = true;
        else
            CharMoving = false;
        ChangeMovementState();

        // записываем в переменную поворот персонажа вокруг оси Z
        float ActualRotationYaw = GetActorRotation().Yaw;

        // записываем в переменную контроллер персонажа
        APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

        if (myController && MovementState != EMovementState::SprintRun_State)
        {
            // создаем переменную для точки куда указывает курсор
            FHitResult ResultHit;

            // получаем точки соприкосновения курсора по созданноу каналу
            //  myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);
            myController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);

            // получаем угол поворота в сторону курсора и записываем в переменную
            float FindRotatorResultYaw =
                UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;

            // вычисляем разницу в углах поворота в сторону курсора и персонажа
            float YawDifference = FindRotatorResultYaw - ActualRotationYaw;

            // коррекция определения ротации (в градусах)
            //  if (YawDifference < -180)
            //  YawDifference += 360;
            //  else if (YawDifference > 180)
            //  YawDifference -= 360;

            // сглаживание поворота персонажа
            //  if (-RotationChangeStep >= YawDifference)
            //  AddActorLocalRotation(FQuat(FRotator(0.0f, -RotationChangeStep, 0.0f)));
            //  else if (YawDifference >= RotationChangeStep)
            //  AddActorLocalRotation(FQuat(FRotator(0.0f, RotationChangeStep, 0.0f)));
            //  else
            SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));

            // указываем оружию стрелять всторону расположения курсора
            if (CurrentWeapon)
            {
                // переменная высоты от пола до оружия
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
                // передаем в оружие точку курсора + высоту над полом
                CurrentWeapon->ShootEndLocation = ResultHit.Location + Displacement;
                // aim cursor like 3d Widget?
            }
        }

        // реализация выносливости

        // если включен спринт и персонжа двигается
        if (MovementState == EMovementState::SprintRun_State && (AxisX != 0 || AxisY != 0))
        {
            // если выносливость > скорости бега
            if (SprintRunStamina > MovementSpeedInfo.RunSpeedNormal)
            {
                // уменьшаем выносливость
                SprintRunStamina -= StaminaStepDown;
            }
            else
            {
                // переходим на бег
                SprintRunEnabled = false;
                MovementState = EMovementState::Run_State;
                CharacterUpdate();
            }
            // если выносливость < скорости спринта
            /*if (SprintRunStamina < MovementSpeedInfo.SprintRunSpeedRun)
            {
                ResSpeed = SprintRunStamina;
                GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
            }*/
        }
        // восстанавливаем выносливость
        else
        {
            if (SprintRunStamina < SprintRunStaminaUpperLimit)
            {
                SprintRunStamina += StaminaStepUp;
            }
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

// функция спавна оружия
void AOnePsychoCharacter::InitWeapon(
    FName IdWeapon, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon)
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
                    // myWeapon->AdditionalWeaponInfo.Round = myWeaponInfo.MaxRound;

                    myWeapon->ReloadTime = myWeaponInfo.ReloadTime;
                    myWeapon->UpdateStateWeapon(MovementState);

                    myWeapon->AdditionalWeaponInfo = WeaponAdditionalInfo;
                    // if (CharacterInventoryComponent)
                    CurrentIndexWeapon = NewCurrentIndexWeapon;
                    // CurrentIndexWeapon = CharacterInventoryComponent->GetWeaponIndexSlotByName(IdWeapon);

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

// функции на инпут стрельбы
void AOnePsychoCharacter::InputAttackPressed()
{
    if (bIsAlive)
    {
        AttackCharEvent(true);
    }
}
void AOnePsychoCharacter::InputAttackReleased()
{
    AttackCharEvent(false);
}

TArray<UStateEffect*> AOnePsychoCharacter::GetCurrentEffectsOnChar()
{
    return Effects;
}

int32 AOnePsychoCharacter::GetCurrentWeaponIndex()
{
    return CurrentIndexWeapon;
}

// функция стрельбы
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
    if (bIsAlive && CurrentWeapon && !CurrentWeapon->WeaponReloading)
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

bool AOnePsychoCharacter::TrySwitchWeaponToIndexByKeyInput(int32 ToIndex)
{
    bool bIsSuccess = false;
    if (CurrentWeapon && !CurrentWeapon->WeaponReloading &&
        CharacterInventoryComponent->WeaponSlots.IsValidIndex(ToIndex))
    {
        if (CurrentIndexWeapon != ToIndex && CharacterInventoryComponent)
        {
            int32 OldIndex = CurrentIndexWeapon;
            FAdditionalWeaponInfo OldInfo;

            if (CurrentWeapon)
            {
                OldInfo = CurrentWeapon->AdditionalWeaponInfo;
                if (CurrentWeapon->WeaponReloading)
                    CurrentWeapon->CancelReload();
            }

            bIsSuccess = CharacterInventoryComponent->SwitchWeaponByIndex(ToIndex, OldIndex, OldInfo);
        }
    }
    return bIsSuccess;
}

void AOnePsychoCharacter::DropCurrentWeapon()
{
    if (CharacterInventoryComponent)
    {
        FDropItem ItemInfo;
        CharacterInventoryComponent->DropWeapobByIndex(CurrentIndexWeapon, ItemInfo);
    }
}

void AOnePsychoCharacter::TrySwitchNextWeapon()
{
    if (CurrentWeapon && !CurrentWeapon->WeaponReloading && CharacterInventoryComponent->WeaponSlots.Num() > 1)
    {
        // We have more then one weapon go switch
        int8 OldIndex = CurrentIndexWeapon;
        FAdditionalWeaponInfo OldInfo;
        if (CurrentWeapon)
        {
            OldInfo = CurrentWeapon->AdditionalWeaponInfo;
            if (CurrentWeapon->WeaponReloading)
                CurrentWeapon->CancelReload();
        }

        if (CharacterInventoryComponent)
        {
            if (CharacterInventoryComponent->SwitchWeaponToIndexByNextPreviosIndex(
                    CurrentIndexWeapon + 1, OldIndex, OldInfo, true))
            {
            }
        }
    }
}

void AOnePsychoCharacter::TrySwitchPreviosWeapon()
{
    // We have more then one weapon go switch
    if (CurrentWeapon && !CurrentWeapon->WeaponReloading && CharacterInventoryComponent->WeaponSlots.Num() > 1)
    {
        int8 OldIndex = CurrentIndexWeapon;
        FAdditionalWeaponInfo OldInfo;
        if (CurrentWeapon)
        {
            // записываем инфо текущего оружия
            OldInfo = CurrentWeapon->AdditionalWeaponInfo;
            if (CurrentWeapon->WeaponReloading)
                CurrentWeapon->CancelReload();
        }

        if (CharacterInventoryComponent)
        {
            // InventoryComponent->SetAdditionalInfoWeapon(OldIndex, GetCurrentWeapon()->AdditionalWeaponInfo);
            if (CharacterInventoryComponent->SwitchWeaponToIndexByNextPreviosIndex(
                    CurrentIndexWeapon - 1, OldIndex, OldInfo, false))
            {
            }
        }
    }
}

void AOnePsychoCharacter::TryAbilityEnabled()
{
    if (AbilityEffect)
    {
        UStateEffect* NewEffect = NewObject<UStateEffect>(this, AbilityEffect);
        if (NewEffect)
        {
            NewEffect->InitObject(this, NAME_None);
        }
    }
}

void AOnePsychoCharacter::InvulnerabilityEnabled()
{
    if (InvulnerabilityEffect)
    {
        UStateEffect* NewEffect = NewObject<UStateEffect>(this, InvulnerabilityEffect);
        if (NewEffect)
        {
            NewEffect->InitObject(this, NAME_None);
        }
    }
}

EPhysicalSurface AOnePsychoCharacter::GetSurfaceType()
{
    EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;
    if (CharHealthComponent)
    {
        if (CharHealthComponent->GetCurrentShield() <= 0)
        {
            if (GetMesh())
            {
                UMaterialInterface* myMaterial = GetMesh()->GetMaterial(0);
                if (myMaterial)
                {
                    Result = myMaterial->GetPhysicalMaterial()->SurfaceType;
                }
            }
        }
    }
    return Result;
}

TArray<UStateEffect*> AOnePsychoCharacter::GetAllCurrentEffects()
{
    return Effects;
}

void AOnePsychoCharacter::RemoveEffect(UStateEffect* RemoveEffect)
{
    Effects.Remove(RemoveEffect);
}

void AOnePsychoCharacter::AddEffect(UStateEffect* newEffect)
{
    Effects.Add(newEffect);
}

void AOnePsychoCharacter::CharDead_BP_Implementation()
{
    // BP
}

void AOnePsychoCharacter::CharDead()
{
    float TimeAnim = 0.0f;
    int32 rnd = FMath::RandHelper(DeadsAnim.Num());
    if (DeadsAnim.IsValidIndex(rnd) && DeadsAnim[rnd] && GetMesh() && GetMesh()->GetAnimInstance())
    {
        // получаем время проигрывания анимации
        TimeAnim = DeadsAnim[rnd]->GetPlayLength();
        GetMesh()->GetAnimInstance()->Montage_Play(DeadsAnim[rnd]);
    }

    bIsAlive = false;

    if (GetController())
    {
        GetController()->UnPossess();
    }

    // UnPossessed();

    GetWorldTimerManager().SetTimer(
        TimerHandle_RagDollTimer, this, &AOnePsychoCharacter::EnableRagdoll, TimeAnim, false);

    GetCursorToWorld()->SetVisibility(false);

    AttackCharEvent(false);

    CharDead_BP();
}

void AOnePsychoCharacter::EnableRagdoll()
{
    if (GetMesh())
    {
        GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
        GetMesh()->SetSimulatePhysics(true);
    }
}

bool AOnePsychoCharacter::GetIsAlive()
{
    return bIsAlive;
}

float AOnePsychoCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (bIsAlive)
    {
        CharHealthComponent->ChangeHealthValue(-DamageAmount);
    }

    // если идет урон по радиусу
    if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
    {
        AProjectileDefault* myProjectile = Cast<AProjectileDefault>(DamageCauser);
        if (myProjectile)
        {
            UTypes::AddEffectBySurfaceType(this, NAME_None, myProjectile->ProjectileSetting.Effect, GetSurfaceType());
        }
    }
    return ActualDamage;
}
