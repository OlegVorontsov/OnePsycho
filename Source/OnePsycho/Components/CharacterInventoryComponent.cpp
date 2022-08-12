// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterInventoryComponent.h"
#include "OnePsychoGameInstance.h"

UCharacterInventoryComponent::UCharacterInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

//удаление слотов с невалидным оружием и назначение оружия в слот 0
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
                    // WeaponSlots.RemoveAt(i);
                    // i--;
                }
            }
        }
    }

    //кол-во слотов = кол-ву элементов в массиве оружия
    MaxSlotsWeapon = WeaponSlots.Num();

    if (WeaponSlots.IsValidIndex(0))
    {
        if (!WeaponSlots[0].NameItem.IsNone())
            OnSwitchWeapon.Broadcast(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalInfo, 0);
    }
}

void UCharacterInventoryComponent::TickComponent(
    float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

//смена оружия по индексу
bool UCharacterInventoryComponent::SwitchWeaponToIndex(
    int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward)
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
    int32 NewCurrentIndex = 0;

    if (WeaponSlots.IsValidIndex(CorrectIndex))
    {
        if (!WeaponSlots[CorrectIndex].NameItem.IsNone())
        {
            if (WeaponSlots[CorrectIndex].AdditionalInfo.Round > 0)
            {
                // good weapon have ammo start change
                bIsSuccess = true;
            }
            else
            {
                UOnePsychoGameInstance* myGI = Cast<UOnePsychoGameInstance>(GetWorld()->GetGameInstance());
                if (myGI)
                {
                    //пробегаемся по слотам с боеприпасами
                    FWeaponInfo myInfo;
                    myGI->GetWeaponInfoByName(WeaponSlots[CorrectIndex].NameItem, myInfo);

                    bool bIsFind = false;
                    int8 j = 0;
                    while (j < AmmoSlots.Num() && !bIsFind)
                    {
                        if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
                        {
                            // good weapon have ammo start change
                            bIsSuccess = true;
                            bIsFind = true;
                        }
                        j++;
                    }
                }
            }
            if (bIsSuccess)
            {
                NewCurrentIndex = CorrectIndex;
                NewIdWeapon = WeaponSlots[CorrectIndex].NameItem;
                NewAdditionalInfo = WeaponSlots[CorrectIndex].AdditionalInfo;
            }
        }
    }

    //для оружия нет патронов в арсенале
    if (!bIsSuccess)
    {
        if (bIsForward)
        {
            int8 iteration = 0;
            int8 Seconditeration = 0;
            //проходимся по слотам
            while (iteration < WeaponSlots.Num() && !bIsSuccess)
            {
                iteration++;
                int8 tmpIndex = ChangeToIndex + iteration;
                if (WeaponSlots.IsValidIndex(tmpIndex))
                {
                    if (!WeaponSlots[tmpIndex].NameItem.IsNone())
                    {
                        if (WeaponSlots[tmpIndex].AdditionalInfo.Round > 0)
                        {
                            // WeaponGood - есть патроны в оружии
                            bIsSuccess = true;
                            NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
                            NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
                            NewCurrentIndex = tmpIndex;
                        }
                        else
                        {
                            FWeaponInfo myInfo;
                            UOnePsychoGameInstance* myGI = Cast<UOnePsychoGameInstance>(GetWorld()->GetGameInstance());

                            myGI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, myInfo);

                            bool bIsFind = false;
                            int8 j = 0;
                            while (j < AmmoSlots.Num() && !bIsFind)
                            {
                                if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
                                {
                                    // WeaponGood - есть патроны в инвентаре
                                    bIsSuccess = true;
                                    NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
                                    NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
                                    NewCurrentIndex = tmpIndex;
                                    bIsFind = true;
                                }
                                j++;
                            }
                        }
                    }
                }
                else
                {
                    // go to end of right of array weapon slots
                    if (OldIndex != Seconditeration)
                    {
                        if (WeaponSlots.IsValidIndex(Seconditeration))
                        {
                            if (!WeaponSlots[Seconditeration].NameItem.IsNone())
                            {
                                if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
                                {
                                    // WeaponGood
                                    bIsSuccess = true;
                                    NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
                                    NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
                                    NewCurrentIndex = Seconditeration;
                                }
                                else
                                {
                                    FWeaponInfo myInfo;
                                    UOnePsychoGameInstance* myGI =
                                        Cast<UOnePsychoGameInstance>(GetWorld()->GetGameInstance());

                                    myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

                                    bool bIsFind = false;
                                    int8 j = 0;
                                    while (j < AmmoSlots.Num() && !bIsFind)
                                    {
                                        if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
                                        {
                                            // WeaponGood
                                            bIsSuccess = true;
                                            NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
                                            NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
                                            NewCurrentIndex = Seconditeration;
                                            bIsFind = true;
                                        }
                                        j++;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        // go to same weapon when start
                        if (WeaponSlots.IsValidIndex(Seconditeration))
                        {
                            if (!WeaponSlots[Seconditeration].NameItem.IsNone())
                            {
                                if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
                                {
                                    // WeaponGood - есть патроны, it same weapon do nothing
                                }
                                else
                                {
                                    FWeaponInfo myInfo;
                                    UOnePsychoGameInstance* myGI =
                                        Cast<UOnePsychoGameInstance>(GetWorld()->GetGameInstance());

                                    myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

                                    bool bIsFind = false;
                                    int8 j = 0;
                                    while (j < AmmoSlots.Num() && !bIsFind)
                                    {
                                        if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
                                        {
                                            if (AmmoSlots[j].Cout > 0)
                                            {
                                                // WeaponGood, it same weapon do nothing
                                            }
                                            else
                                            {
                                                // Not find weapon with amm need init Pistol with infinity ammo
                                                UE_LOG(LogTemp, Error,
                                                    TEXT("UTPSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - "
                                                         "NEED"));
                                            }
                                        }
                                        j++;
                                    }
                                }
                            }
                        }
                    }
                    Seconditeration++;
                }
            }
        }
        //идем с другой стороны массива
        else
        {
            int8 iteration = 0;
            int8 Seconditeration = WeaponSlots.Num() - 1;
            while (iteration < WeaponSlots.Num() && !bIsSuccess)
            {
                iteration++;
                int8 tmpIndex = ChangeToIndex - iteration;
                if (WeaponSlots.IsValidIndex(tmpIndex))
                {
                    if (!WeaponSlots[tmpIndex].NameItem.IsNone())
                    {
                        if (WeaponSlots[tmpIndex].AdditionalInfo.Round > 0)
                        {
                            // WeaponGood
                            bIsSuccess = true;
                            NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
                            NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
                            NewCurrentIndex = tmpIndex;
                        }
                        else
                        {
                            FWeaponInfo myInfo;
                            UOnePsychoGameInstance* myGI = Cast<UOnePsychoGameInstance>(GetWorld()->GetGameInstance());

                            myGI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, myInfo);

                            bool bIsFind = false;
                            int8 j = 0;
                            while (j < AmmoSlots.Num() && !bIsFind)
                            {
                                if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
                                {
                                    // WeaponGood
                                    bIsSuccess = true;
                                    NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
                                    NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
                                    NewCurrentIndex = tmpIndex;
                                    bIsFind = true;
                                }
                                j++;
                            }
                        }
                    }
                }
                else
                {
                    // go to end of LEFT of array weapon slots
                    if (OldIndex != Seconditeration)
                    {
                        if (WeaponSlots.IsValidIndex(Seconditeration))
                        {
                            if (!WeaponSlots[Seconditeration].NameItem.IsNone())
                            {
                                if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
                                {
                                    // WeaponGood
                                    bIsSuccess = true;
                                    NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
                                    NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
                                    NewCurrentIndex = Seconditeration;
                                }
                                else
                                {
                                    FWeaponInfo myInfo;
                                    UOnePsychoGameInstance* myGI =
                                        Cast<UOnePsychoGameInstance>(GetWorld()->GetGameInstance());

                                    myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

                                    bool bIsFind = false;
                                    int8 j = 0;
                                    while (j < AmmoSlots.Num() && !bIsFind)
                                    {
                                        if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
                                        {
                                            // WeaponGood
                                            bIsSuccess = true;
                                            NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
                                            NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
                                            NewCurrentIndex = Seconditeration;
                                            bIsFind = true;
                                        }
                                        j++;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        // go to same weapon when start
                        if (WeaponSlots.IsValidIndex(Seconditeration))
                        {
                            if (!WeaponSlots[Seconditeration].NameItem.IsNone())
                            {
                                if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
                                {
                                    // WeaponGood, it same weapon do nothing
                                }
                                else
                                {
                                    FWeaponInfo myInfo;
                                    UOnePsychoGameInstance* myGI =
                                        Cast<UOnePsychoGameInstance>(GetWorld()->GetGameInstance());

                                    myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

                                    bool bIsFind = false;
                                    int8 j = 0;
                                    while (j < AmmoSlots.Num() && !bIsFind)
                                    {
                                        if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
                                        {
                                            if (AmmoSlots[j].Cout > 0)
                                            {
                                                // WeaponGood, it same weapon do nothing
                                            }
                                            else
                                            {
                                                // Not find weapon with amm need init Pistol with infinity ammo
                                                UE_LOG(LogTemp, Error,
                                                    TEXT("UTPSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - "
                                                         "NEED"));
                                            }
                                        }
                                        j++;
                                    }
                                }
                            }
                        }
                    }
                    Seconditeration--;
                }
            }
        }
    }

    if (bIsSuccess)
    {
        SetAdditionalInfoWeapon(OldIndex, OldInfo);
        OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo, NewCurrentIndex);
        // OnWeaponAmmoAviable.Broadcast();
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

FName UCharacterInventoryComponent::GetWeaponNameBySlotIndex(int32 indexSlot)
{
    FName result;
    if (WeaponSlots.IsValidIndex(indexSlot))
    {
        result = WeaponSlots[indexSlot].NameItem;
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

                OnWeaponAdditionalInfoChange.Broadcast(IndexWeapon, NewInfo);
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

//добавление патронов в слот с патронами
void UCharacterInventoryComponent::AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CoutChangeAmmo)
{
    bool bIsFind = false;
    int8 i = 0;
    while (i < AmmoSlots.Num() && !bIsFind)
    {
        if (AmmoSlots[i].WeaponType == TypeWeapon)
        {
            AmmoSlots[i].Cout += CoutChangeAmmo;
            if (AmmoSlots[i].Cout > AmmoSlots[i].MaxCout)
                AmmoSlots[i].Cout = AmmoSlots[i].MaxCout;

            OnAmmoChange.Broadcast(AmmoSlots[i].WeaponType, AmmoSlots[i].Cout);

            bIsFind = true;
        }
        i++;
    }
}

bool UCharacterInventoryComponent::CheckAmmoForWeapon(EWeaponType TypeWeapon, int8& AviableAmmoForWeapon)
{
    AviableAmmoForWeapon = 0;
    bool bIsFind = false;
    int8 i = 0;
    while (i < AmmoSlots.Num() && !bIsFind)
    {
        if (AmmoSlots[i].WeaponType == TypeWeapon)
        {
            bIsFind = true;
            AviableAmmoForWeapon = AmmoSlots[i].Cout;
            if (AmmoSlots[i].Cout > 0)
            {
                // remove not here, only when pickUp ammo this type, or swithc weapon
                OnWeaponAmmoAviable.Broadcast(TypeWeapon);
                return true;
            }
        }
        i++;
    }
    //патроны закончились
    OnWeaponAmmoEmpty.Broadcast(TypeWeapon);

    return false;
}

//функция проверки нужны ли боеприпасы
bool UCharacterInventoryComponent::CheckCanTakeAmmo(EWeaponType AmmoType)
{
    bool result = false;
    int8 i = 0;
    while (i < AmmoSlots.Num() && !result)
    {
        if (AmmoSlots[i].WeaponType == AmmoType && AmmoSlots[i].Cout < AmmoSlots[i].MaxCout)
            result = true;
        i++;
    }
    return true;
}

//функция поиска пустого слота с оружием
bool UCharacterInventoryComponent::CheckCanTakeWeapon(int32& FreeSlot)
{
    bool bIsFreeSlot = false;
    int8 i = 0;
    while (i < WeaponSlots.Num() && !bIsFreeSlot)
    {
        if (WeaponSlots[i].NameItem.IsNone())
        {
            bIsFreeSlot = true;
            //сохраняем пустой слот
            FreeSlot = i;
        }
        i++;
    }
    return bIsFreeSlot;
}

bool UCharacterInventoryComponent::SwitchWeaponToInventory(
    FWeaponSlot NewWeapon, int32 IndexSlot, int32 CurrentIndexWeaponChar, FDropItem& DropItemInfo)
{
    bool result = false;

    if (WeaponSlots.IsValidIndex(IndexSlot) && GetDropItemInfoFromInventory(IndexSlot, DropItemInfo))
    {
        WeaponSlots[IndexSlot] = NewWeapon;
        SwitchWeaponToIndex(CurrentIndexWeaponChar, -1, NewWeapon.AdditionalInfo, true);

        //обновляем виджет оружия
        OnUpdateWeaponSlots.Broadcast(IndexSlot, NewWeapon);

        result = true;
    }
    return result;
}

//функция пикапа оружия в пустой слот оружия
bool UCharacterInventoryComponent::TryGetWeaponToInventory(FWeaponSlot NewWeapon)
{
    int32 indexSlot = -1;
    if (CheckCanTakeWeapon(indexSlot))
    {
        if (WeaponSlots.IsValidIndex(indexSlot))
        {
            WeaponSlots[indexSlot] = NewWeapon;

            OnUpdateWeaponSlots.Broadcast(indexSlot, NewWeapon);
            return true;
        }
    }
    return false;
}

//функция сброса текущего оружия
bool UCharacterInventoryComponent::GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem& DropItemInfo)
{
    bool result = false;

    FName DropItemName = GetWeaponNameBySlotIndex(IndexSlot);

    UOnePsychoGameInstance* myGI = Cast<UOnePsychoGameInstance>(GetWorld()->GetGameInstance());
    if (myGI)
    {
        result = myGI->GetDropItemInfoByWeaponName(DropItemName, DropItemInfo);
        if (WeaponSlots.IsValidIndex(IndexSlot))
        {
            DropItemInfo.WeaponInfo.AdditionalInfo = WeaponSlots[IndexSlot].AdditionalInfo;
        }
    }
    return result;
}
