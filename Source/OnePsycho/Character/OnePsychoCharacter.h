// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "OnePsycho/FuncLibrary/Types.h"

#include "OnePsychoCharacter.generated.h"

UCLASS(Blueprintable)
class AOnePsychoCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AOnePsychoCharacter();

    // Called every frame.
    virtual void Tick(float DeltaSeconds) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /** Returns TopDownCameraComponent subobject **/
    FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** Returns CursorToWorld subobject **/
    FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

private:
    /** Top down camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* TopDownCameraComponent;

    /** Camera boom positioning the camera above the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    /** A decal that projects to the cursor location. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UDecalComponent* CursorToWorld;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    EMovementState MovementState = EMovementState::Run_State;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FCharacterSpeed MovementSpeedInfo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool SprintRunEnabled = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool WalkEnabled = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool AimEnabled = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float RotationChangeStep = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float StaminaStepDown = 0.5;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float StaminaStepUp = 0.5;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float BufferSprintRunStamina = 200;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float SprintRunStamina = MovementSpeedInfo.SprintRunSpeedRun;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float SprintRunStaminaUpperLimit = MovementSpeedInfo.SprintRunSpeedRun + BufferSprintRunStamina;

    UFUNCTION()
    void InputAxisX(float Value);
    UFUNCTION()
    void InputAxisY(float Value);

    float AxisX = 0.0f;
    float AxisY = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float ResSpeed = MovementSpeedInfo.RunSpeedNormal;

    // Tick Func
    UFUNCTION()
    void MovementTick(float DeltaTime);

    UFUNCTION(BlueprintCallable)
    void CharacterUpdate();
    UFUNCTION(BlueprintCallable)
    void ChangeMovementState();
};

// test
// test 2
