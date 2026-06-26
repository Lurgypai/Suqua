#pragma once 
#include "EntitySystem.h"
#include "Color.h"
#include "Vec2.h"

struct EntityAddRectGFXArgs {
    Color c;
    bool filled;
    Vec2f res;
};

extern void EntityAddRectGFX(EntityId id, const EntityAddRectGFXArgs& args);
