// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types.h"
#include "CharacterInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnSwitchWeapon, FName, WeaponIdName, FAdditionalWeaponInfo, WeaponAdditionalInfo, int32, NewCurrentIndexWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChange, EWeaponType, TypeAmmo, int32, Cout);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnWeaponAdditionalInfoChange, int32, IndexSlot, FAdditionalWeaponInfo, AdditionalInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoEmpty, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoAviable, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateWeaponSlots, int32, IndexSlotChange, FWeaponSlot, NewInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponNotHaveRound, int32, IndexSlotWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponHaveRound, int32, IndexSlotWeapon);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ONEPSYCHO_API UCharacterInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCharacterInventoryComponent();

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnSwitchWeapon OnSwitchWeapon;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnAmmoChange OnAmmoChange;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnWeaponAdditionalInfoChange OnWeaponAdditionalInfoChange;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnWeaponAmmoEmpty OnWeaponAmmoEmpty;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnWeaponAmmoAviable OnWeaponAmmoAviable;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnUpdateWeaponSlots OnUpdateWeaponSlots;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnWeaponNotHaveRound OnWeaponNotHaveRound;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnWeaponHaveRound OnWeaponHaveRound;

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(
        float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons")
    TArray<FWeaponSlot> WeaponSlots;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons")
    TArray<FAmmoSlot> AmmoSlots;

    int32 MaxSlotsWeapon = 0;

    bool SwitchWeaponToIndexByNextPreviosIndex(
        int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward);
    bool SwitchWeaponByIndex(int32 IndexWeaponToChange, int32 PreviosIndex, FAdditionalWeaponInfo PreviosWeaponInfo);

    FAdditionalWeaponInfo GetAdditionalInfoWeapon(int32 IndexWeapon);
    int32 GetWeaponIndexSlotByName(FName IdWeaponName);
    FName GetWeaponNameBySlotIndex(int32 indexSlot);
    void SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);

    bool GetWeaponTypeByIndexSlot(int32 IndexSlot, EWeaponType& WeaponType);
    bool GetWeaponTypeByNameWeapon(FName IdWeaponName, EWeaponType& WeaponType);

    UFUNCTION(BlueprintCallable)
    void AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CoutChangeAmmo);
    bool CheckAmmoForWeapon(EWeaponType TypeWeapon, int8& AviableAmmoForWeapon);

    // Interface PickUpActors
    UFUNCTION(BlueprintCallable, Category = "Interface")
    bool CheckCanTakeAmmo(EWeaponType AmmoType);

    UFUNCTION(BlueprintCallable, Category = "Interface")
    bool CheckCanTakeWeapon(int32& FreeSlot);

    UFUNCTION(BlueprintCallable, Category = "Interface")
    bool SwitchWeaponToInventory(
        FWeaponSlot NewWeapon, int32 IndexSlot, int32 CurrentIndexWeaponChar, FDropItem& DropItemInfo);

    UFUNCTION(BlueprintCallable, Category = "Interface")
    bool TryGetWeaponToInventory(FWeaponSlot NewWeapon);

    UFUNCTION(BlueprintCallable, Category = "Interface")
    void DropWeapobByIndex(int32 ByIndex, FDropItem& DropItemInfo);

    // функция сброса текущего оружия
    UFUNCTION(BlueprintCallable, Category = "Interface")
    bool GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem& DropItemInfo);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FWeaponSlot> GetWeaponSlots();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FAmmoSlot> GetAmmoSlots();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void InitInventory(TArray<FWeaponSlot> NewWeaponSlotsInfo, TArray<FAmmoSlot> NewAmmoSlotsInfo);
};
