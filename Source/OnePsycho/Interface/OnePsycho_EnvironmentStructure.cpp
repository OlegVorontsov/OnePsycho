
#include "OnePsycho_EnvironmentStructure.h"
#include "Materials/MaterialInterface.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

AOnePsycho_EnvironmentStructure::AOnePsycho_EnvironmentStructure()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AOnePsycho_EnvironmentStructure::BeginPlay()
{
    Super::BeginPlay();
}

void AOnePsycho_EnvironmentStructure::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

EPhysicalSurface AOnePsycho_EnvironmentStructure::GetSurfaceType()
{
    EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;

    UStaticMeshComponent* myMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
    if (myMesh)
    {
        UMaterialInterface* myMaterial = myMesh->GetMaterial(0);
        if (myMaterial)
        {
            Result = myMaterial->GetPhysicalMaterial()->SurfaceType;
        }
    }
    return Result;
}

TArray<UStateEffect*> AOnePsycho_EnvironmentStructure::GetAllCurrentEffects()
{
    return Effects;
}

void AOnePsycho_EnvironmentStructure::RemoveEffect(UStateEffect* RemoveEffect)
{
    Effects.Remove(RemoveEffect);
}

void AOnePsycho_EnvironmentStructure::AddEffect(UStateEffect* newEffect)
{
    Effects.Add(newEffect);
}
