// Fill out your copyright notice in the Description page of Project Settings.

#include "OnePsychoGameInstance.h"

bool UOnePsychoGameInstance::GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo)
{
    bool bIsFind = false;
    FWeaponInfo* WeaponInfoRow;

    if (WeaponInfoTable)
    {
        WeaponInfoRow = WeaponInfoTable->FindRow<FWeaponInfo>(NameWeapon, "", false);
        if (WeaponInfoRow)
        {
            bIsFind = true;
            OutInfo = *WeaponInfoRow;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UOnePsychoGameInstance::GetWeaponInfoByName - WeaponTable -NULL"));
    }
    return bIsFind;
}

bool UOnePsychoGameInstance::GetDropItemInfoByWeaponName(FName NameItem, FDropItem& OutInfo)
{
    bool bIsFind = false;

    if (DropItemInfoTable)
    {
        FDropItem* DropItemInfoRow;
        TArray<FName> RowNames = DropItemInfoTable->GetRowNames();

        int8 i = 0;
        while (i < RowNames.Num() && !bIsFind)
        {
            DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(RowNames[i], "");
            if (DropItemInfoRow->WeaponInfo.NameItem == NameItem)
            {
                OutInfo = (*DropItemInfoRow);
                bIsFind = true;
            }
            i++;
        }
    }
    else
    {
        UE_LOG(
            LogTemp, Warning, TEXT("UOnePsychoGameInstance::GetDropItemInfoByWeaponName - DropItemInfoTable - NULL"));
    }
    return bIsFind;
}

bool UOnePsychoGameInstance::GetDropItemInfoByName(FName NameItem, FDropItem& OutInfo)
{
    bool bIsFind = false;
    FDropItem* DropItemInfoRow;

    if (DropItemInfoTable)
    {
        DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(NameItem, "", false);
        if (DropItemInfoRow)
        {
            bIsFind = true;
            OutInfo = *DropItemInfoRow;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UOnePsychoGameInstance::GetDropItemInfoByName - DropItemTable -NULL"));
    }
    return bIsFind;
}
