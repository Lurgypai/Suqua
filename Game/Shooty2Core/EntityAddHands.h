#pragma once
#include "EntitySystem.h"
#include "Vec2.h"

struct EntityAddHandsArgs {
    Vec2f bodyOffset;
    float armLength;
};

extern void EntityAddHands(EntityId id, const EntityAddHandsArgs& args);
