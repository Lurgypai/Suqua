#pragma once

#include <string>

#include "EntitySystem.h"
#include "Vec2.h"

struct EntityAddHandGFXArgs {
    std::string lHandPath;
    Vec2f lHandOffset;
    std::string rHandPath;
    Vec2f rHandOffset;
};

extern void EntityAddHandGFX(EntityId id, const EntityAddHandGFXArgs& args);
