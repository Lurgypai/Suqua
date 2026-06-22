#pragma once
#include "Vec2.h"
#include "EntitySystem.h"

struct EntityPositionArgs {
    Vec2f pos;
};

extern void EntityAddPosition(EntityId id, const EntityPositionArgs& args);
