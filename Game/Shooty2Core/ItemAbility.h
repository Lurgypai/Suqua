#pragma once
#include "EntitySystem.h"
#include "Scene.h"
#include "Controller.h"

// governs what items do when they're used
class ItemAbility {
public:
    virtual ~ItemAbility() {};
    virtual void update(float delta) = 0;
    virtual void doAbility(Scene& scene, EntityId sourceEntity, EntityId targetEntity,
            const Controller& controller, const Vec2f& heldPos, float angle) = 0;
    virtual std::unique_ptr<ItemAbility> clone() const = 0;
};
