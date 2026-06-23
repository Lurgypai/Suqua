#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "TeamComponent.h"

struct EntityAddBulletArgs {
    Vec2f res;
    TeamComponent::TeamId team;
    int damage;
    int lifetime;
    std::string collisionHandler;
};

extern void EntityAddBullet(EntityId id, const EntityAddBulletArgs& args);
