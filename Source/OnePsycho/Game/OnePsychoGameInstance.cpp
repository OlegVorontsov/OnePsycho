// Fill out your copyright notice in the Description page of Project Settings.

#include "OnePsychoGameInstance.h"

FWeaponInfo UOnePsychoGameInstance::GetWeaponInfoByName(FName NameWeapon)
{
    // bool bIsFind = false;
    FWeaponInfo WeaponInfo;
    return WeaponInfo;

    /*if (WeaponInfoTable)
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

    return bIsFind;*/
}
