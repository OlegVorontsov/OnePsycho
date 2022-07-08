// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterInventoryComponent.h"
#include "OnePsychoGameInstance.h"

UCharacterInventoryComponent::UCharacterInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

//�������� ������ � ���������� ������ � ���������� ������ � ���� 0
void UCharacterInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // ���������� �� ������� ������
    for (int8 i = 0; i < WeaponSlots.Num(); i++)
    {
        //�������� ��� game instance
        UOnePsychoGameInstance* myGI = Cast<UOnePsychoGameInstance>(GetWorld()->GetGameInstance());
        if (myGI)
        {
            //��������� ����� ������ ����� ��������
            if (!WeaponSlots[i].NameItem.IsNone())
            {
                FWeaponInfo Info;

                if (myGI->GetWeaponInfoByName(WeaponSlots[i].NameItem, Info))
                    WeaponSlots[i].AdditionalInfo.Round = Info.MaxRound;
                else
                {
                    //������� ����
                    WeaponSlots.RemoveAt(i);
                    i--;
                }
            }
        }
    }

    //���-�� ������ = ���-�� ��������� � ������� ������
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

//����� ������ �� �������
bool UCharacterInventoryComponent::SwitchWeaponToIndex(
    int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo) // bool bIsForward)
{
    bool bIsSuccess = false;
    int8 CorrectIndex = ChangeToIndex;

    //������������ ����� �� ������ �� ������� �������
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
        //��������� ������ ������� ������
        SetAdditionalInfoWeapon(OldIndex, OldInfo);
        OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo);
    }
    return bIsSuccess;
}

//��������� ���-�� �������� ������
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

//��������� ������� ������ �� �����
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

//��������� ���-�� �������� ������
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
