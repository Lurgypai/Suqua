#pragma once

#include "EntitySystem.h"
#include "Vec2.h"

struct EntityAddPhysicsArgs {
    Vec2f pos;
    Vec2f res;
    bool collideable;
    bool collideableWith;
    bool centered;
};

extern void EntityAddPhysics(EntityId id, const EntityAddPhysicsArgs& args);
