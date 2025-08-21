#pragma once
#include "ComponentMacros.h"

class TeleportZoneGFXComponent {
    CompMembers(TeleportZoneGFXComponent);
public:
    TeleportZoneGFXComponent(EntityId id_);
    void update(int timeDelta);
private:
    int particleSpawnDelay;
    int particleSpawnElapsed;
};
