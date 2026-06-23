#pragma once
#include "Vec2.h"
#include "EntitySystem.h"
#include "TeamComponent.h"

struct EntityAddLivingArgs {
    float moveSpeed;
    TeamComponent::TeamId team;
    Vec2f hurtboxOffset;
    Vec2f hurtboxRes;
    std::int32_t health;
    std::string deathCallback;
};

extern void EntityAddLiving(EntityId id, const EntityAddLivingArgs& args);
