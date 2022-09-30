
#include "OnePsycho_IGameActor.h"

EPhysicalSurface IOnePsycho_IGameActor::GetSurfaceType()
{
    return EPhysicalSurface::SurfaceType_Default;
}

TArray<UStateEffect*> IOnePsycho_IGameActor::GetAllCurrentEffects()
{
    TArray<UStateEffect*> Effect;

    return Effect;
}

void IOnePsycho_IGameActor::RemoveEffect(UStateEffect* RemoveEffect) {}

void IOnePsycho_IGameActor::AddEffect(UStateEffect* newEffect) {}
