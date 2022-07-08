// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterInventoryComponent.h"
#include "OnePsychoGameInstance.h"

UCharacterInventoryComponent::UCharacterInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

//удаление слотов с невалидным оружие и назначение оружия в слот 0
void UCharacterInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // проходимся по массиву оружия
    for (int8 i = 0; i < WeaponSlots.Num(); i++)
    {
        //получаем наш game instance
        UOnePsychoGameInstance* myGI = Cast<UOnePsychoGameInstance>(GetWorld()->GetGameInstance());
        if (myGI)
        {
            //проверяем чтобы оружие имело название
            if (!WeaponSlots[i].NameItem.IsNone())
            {
                FWeaponInfo Info;

                if (myGI->GetWeaponInfoByName(WeaponSlots[i].NameItem, Info))
                    WeaponSlots[i].AdditionalInfo.Round = Info.MaxRound;
                else
                {
                    //удаляем слот
                    WeaponSlots.RemoveAt(i);
                    i--;
                }
            }
        }
    }

    //кол-во слотов = кол-ву элементов в массиве оружия
    MaxSlotsWeapon = WeaponSlots.Num();

    if (WeaponSlots.IsValidIndex(0))
    {
        if (!WeaponSlots[0].NameItem.IsNone())
            OnSwitchWeapon.Broadcast(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalInfo); // 0);
    }
}

void UCharacterInventoryComponent::TickComponent(
    float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

//смена оружия по индексу
bool UCharacterInventoryComponent::SwitchWeaponToIndex(
    int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo) // bool bIsForward)
{
    bool bIsSuccess = false;
    int8 CorrectIndex = ChangeToIndex;

    //ограниечения чтобы не ввыйти за границы массива
    if (ChangeToIndex > WeaponSlots.Num() - 1)
        CorrectIndex = 0;
    else if (ChangeToIndex < 0)
        CorrectIndex = WeaponSlots.Num() - 1;

    FName NewIdWeapon;
    FAdditionalWeaponInfo NewAdditionalInfo;

    int8 i = 0;
    while (i < WeaponSlots.Num() && !bIsSuccess)
    {
        if (i == CorrectIndex)
        {
            if (WeaponSlots[i].NameItem.IsNone())
            {
                NewIdWeapon = WeaponSlots[i].NameItem;
                NewAdditionalInfo = WeaponSlots[i].AdditionalInfo;
                bIsSuccess = true;
            }
        }
        i++;
    }
    if (!bIsSuccess)
    {
    }
    if (bIsSuccess)
    {
        //сохраняем данные которые меняем
        SetAdditionalInfoWeapon(OldIndex, OldInfo);
        OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo);
    }
    return bIsSuccess;
}

//получение кол-ва патронов оружия
FAdditionalWeaponInfo UCharacterInventoryComponent::GetAdditionalInfoWeapon(int32 IndexWeapon)
{
    FAdditionalWeaponInfo result;
    if (WeaponSlots.IsValidIndex(IndexWeapon))
    {
        bool bIsFind = false;
        int8 i = 0;
        while (i < WeaponSlots.Num() && !bIsFind)
        {
            if (i == IndexWeapon)
            {
                result = WeaponSlots[i].AdditionalInfo;
                bIsFind = true;
            }
            i++;
        }
        if (!bIsFind)
            UE_LOG(LogTemp, Warning,
                TEXT("UCharacterInventoryComponent::GetAdditionalInfoWeapon - No Found Weapon with index - %d"),
                IndexWeapon);
    }
    else
        UE_LOG(LogTemp, Warning,
            TEXT("UCharacterInventoryComponent::GetAdditionalInfoWeapon - Not Correct index Weapon - %d"), IndexWeapon);

    return result;
}

//получение индекса оружия по имени
int32 UCharacterInventoryComponent::GetWeaponIndexSlotByName(FName IdWeaponName)
{
    int32 result = -1;
    int8 i = 0;
    bool bIsFind = false;
    while (i < WeaponSlots.Num() && !bIsFind)
    {
        if (WeaponSlots[i].NameItem == IdWeaponName)
        {
            bIsFind = true;
            result = i;
        }
        i++;
    }
    return result;
}

//установка кол-ва патронов оружию
void UCharacterInventoryComponent::SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo)
{
    if (WeaponSlots.IsValidIndex(IndexWeapon))
    {
        bool bIsFind = false;
        int8 i = 0;
        while (i < WeaponSlots.Num() && !bIsFind)
        {
            if (i == IndexWeapon)
            {
                WeaponSlots[i].AdditionalInfo = NewInfo;
                bIsFind = true;

                // OnWeaponAdditionalInfoChange.Broadcast(IndexWeapon, NewInfo);
            }
            i++;
        }
        if (!bIsFind)
            UE_LOG(LogTemp, Warning,
                TEXT("UCharacterInventoryComponent::SetAdditionalInfoWeapon - No Found Weapon with index - %d"),
                IndexWeapon);
    }
    else
        UE_LOG(LogTemp, Warning,
            TEXT("UCharacterInventoryComponent::SetAdditionalInfoWeapon - Not Correct index Weapon - %d"), IndexWeapon);
}
