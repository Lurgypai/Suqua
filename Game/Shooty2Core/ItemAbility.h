#pragma once
#include "EntitySystem.h"
#include "InventoryItem.h"
#include "Scene.h"

class InventoryItem;

// governs what items do when they're used
class ItemAbility {
public:
    virtual void update(float delta) = 0;
    virtual void doAbility(Scene& scene, EntityId sourceEntity, Vec2f stick1, Vec2f stick2, InventoryItem& sourceInvItem) = 0;
    virtual std::unique_ptr<ItemAbility> clone() const = 0;
private:
};
