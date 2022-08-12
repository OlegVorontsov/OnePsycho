// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Types.h"
#include "Engine/DataTable.h"
#include "WeaponDefault.h"

#include "OnePsychoGameInstance.generated.h"

UCLASS()
class ONEPSYCHO_API UOnePsychoGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    // table
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " WeaponSetting ")
    UDataTable* WeaponInfoTable = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " DropWeaponSetting ")
    UDataTable* DropItemInfoTable = nullptr;

    UFUNCTION(BlueprintCallable)
    bool GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo);

    UFUNCTION(BlueprintCallable)
    bool GetDropItemInfoByWeaponName(FName NameItem, FDropItem& OutInfo);

    UFUNCTION(BlueprintCallable)
    bool GetDropItemInfoByName(FName NameItem, FDropItem& OutInfo);
};
