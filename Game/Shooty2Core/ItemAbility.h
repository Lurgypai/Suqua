#pragma once
#include "EntitySystem.h"
#include "InventoryItem.h"
#include "Scene.h"
#include "Controller.h"

class InventoryItem;

// governs what items do when they're used
class ItemAbility {
public:
    virtual void update(float delta) = 0;
    virtual void doAbility(Scene& scene, EntityId sourceEntity, const Controller& controller, InventoryItem& sourceInvItem) = 0;
    virtual std::unique_ptr<ItemAbility> clone() const = 0;
    // the entity to be affected when this is activated
    EntityId targetEntity;
};
