#include "EntityAddHands.h"
#include "ControllerComponent.h"
#include "InventoryComponent.h"
#include "HandComponent.h"
#include "AimToLStickComponent.h"

void EntityAddHands(EntityId id, const EntityAddHandsArgs& args) {
    EntitySystem::MakeComps<ControllerComponent>(1, &id);
    EntitySystem::MakeComps<AimToLStickComponent>(1, &id);
    EntitySystem::MakeComps<InventoryComponent>(1, &id);
    EntitySystem::MakeComps<HandComponent>(1, &id,
            args.bodyOffset,
            args.armLength
            );
}
