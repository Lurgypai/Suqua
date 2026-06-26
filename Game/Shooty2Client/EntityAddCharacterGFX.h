#pragma once
#include "EntitySystem.h"
#include "Vec2.h"

struct EntityAddCharacterGFXArgs {
    std::string texPath;
    std::string animJsonPath;
    Vec2f offset;
    Vec2f shadowOffset;
    bool hasUpDown;
};

extern void EntityAddCharacterGFX(EntityId id, const EntityAddCharacterGFXArgs& args);
