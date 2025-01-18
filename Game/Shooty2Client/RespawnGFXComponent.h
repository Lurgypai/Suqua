#pragma once
#include "ComponentMacros.h"
class RespawnGFXComponent {
    CompMembers(RespawnGFXComponent);

public:
    RespawnGFXComponent(EntityId id_);
    void update();
};
