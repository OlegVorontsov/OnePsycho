
#pragma once

#include "CoreMinimal.h"
#include "OnePsychoHealthComponent.h"
#include "OnePsychoCharHealthComponent.generated.h"

UCLASS()
class ONEPSYCHO_API UOnePsychoCharHealthComponent : public UOnePsychoHealthComponent
{
    GENERATED_BODY()

public:
    void ChangeCurrentHealth(float ChangeValue) override;
};
