
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OnePsycho/FuncLibrary/Types.h"
#include "OnePsychoCharHealthComponent.h"
#include "OnePsycho_IGameActor.h"
#include "StateEffect.h"
#include "OnePsychoCharacter.generated.h"

class AWeaponDefault;

UCLASS(Blueprintable)
class AOnePsychoCharacter : public ACharacter, public IOnePsycho_IGameActor
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    AOnePsychoCharacter();

    FTimerHandle TimerHandle_RagDollTimer;

    // Called every frame.
    virtual void Tick(float DeltaSeconds) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

    /** Returns TopDownCameraComponent subobject **/
    FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
    class UCharacterInventoryComponent* CharacterInventoryComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
    class UOnePsychoCharHealthComponent* CharHealthComponent;

    /** Returns CursorToWorld subobject **/
    // FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

private:
    /** Top down camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* TopDownCameraComponent;

    /** Camera boom positioning the camera above the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    /** A decal that projects to the cursor location. */
    // UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    // class UDecalComponent* CursorToWorld;

public:
    // Cursor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
    UMaterialInterface* CursorMaterial = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
    FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

    UDecalComponent* CurrentCursor = nullptr;

    UFUNCTION(BlueprintCallable)
    UDecalComponent* GetCursorToWorld();

    //движение
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    EMovementState MovementState = EMovementState::Aim_State;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FCharacterSpeed MovementSpeedInfo;

    //переменные включения видов движения
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool SprintRunEnabled = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool WalkEnabled = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool AimEnabled = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool CharMoving = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bIsAlive = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    TArray<UAnimMontage*> DeadsAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    TSubclassOf<UStateEffect> AbilityEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Invulnerability")
    TSubclassOf<UStateEffect> InvulnerabilityEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float RotationChangeStep = 5;

    //переменные для реализации выносливости
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float StaminaStepDown = 2.5;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float StaminaStepUp = 2.5;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float BufferSprintRunStamina = 200;

    //выносливость = скорости спринта
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float SprintRunStamina = MovementSpeedInfo.SprintRunSpeedRun;

    //предел выносливости
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float SprintRunStaminaUpperLimit = MovementSpeedInfo.SprintRunSpeedRun + BufferSprintRunStamina;

    //функции движения
    UFUNCTION()
    void InputAxisX(float Value);
    UFUNCTION()
    void InputAxisY(float Value);
    UFUNCTION()
    void MovementTick(float DeltaTime);

    float AxisX = 0.0f;
    float AxisY = 0.0f;

    //переменная результирущей скорости движения
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float ResSpeed = MovementSpeedInfo.RunSpeedNormal;

    //функции изменения скорости движения
    UFUNCTION(BlueprintCallable)
    void CharacterUpdate();
    UFUNCTION(BlueprintCallable)
    void ChangeMovementState();

    // Weapon
    AWeaponDefault* CurrentWeapon = nullptr;

    // Effects
    TArray<UStateEffect*> Effects;

    // for demo
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
    FName InitWeaponName;

    //функция спавна оружия
    UFUNCTION(BlueprintCallable)
    void InitWeapon(FName IdWeapon, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon);

    //функция перезарядки
    UFUNCTION(BlueprintCallable)
    void TryReloadWeapon();

    UFUNCTION(BlueprintCallable)
    AWeaponDefault* GetCurrentWeapon();

    UFUNCTION()
    void InputAttackPressed();
    UFUNCTION()
    void InputAttackReleased();

    UFUNCTION(BlueprintCallable)
    void AttackCharEvent(bool bIsFiring);

    UFUNCTION()
    void WeaponFireStart(UAnimMontage* Anim);
    UFUNCTION()
    void WeaponReloadStart(UAnimMontage* Anim);
    UFUNCTION()
    void WeaponReloadEnd(bool bIsSuccess, int32 AmmoSafe);

    UFUNCTION(BlueprintNativeEvent)
    void WeaponFireStart_BP(UAnimMontage* Anim);
    UFUNCTION(BlueprintNativeEvent)
    void WeaponReloadStart_BP(UAnimMontage* Anim);
    UFUNCTION(BlueprintNativeEvent)
    void WeaponReloadEnd_BP(bool bIsSuccess);

    void TrySwicthNextWeapon();
    void TrySwitchPreviosWeapon();

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    int32 CurrentIndexWeapon = 0;

    // Interface
    EPhysicalSurface GetSurfaceType() override;

    TArray<UStateEffect*> GetAllCurrentEffects() override;

    void RemoveEffect(UStateEffect* RemoveEffect) override;

    void AddEffect(UStateEffect* newEffect) override;

    void TryAbilityEnabled();

    UFUNCTION(BlueprintCallable)
    void InvulnerabilityEnabled();
    // End Interface

    UFUNCTION()
    void CharDead();

    void EnableRagdoll();
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;
};
