#include "EntityAddHands.h"
#include "ControllerComponent.h"
#include "InventoryComponent.h"
#include "HandComponent.h"
#include "AimToLStickComponent.h"
#include "NetworkDataComponent.h"
#include "DirectionComponent.h"
#include "NetworkDataComponentDataFields.h"

void EntityAddHands(EntityId id, const EntityAddHandsArgs& args) {
    EntitySystem::MakeComps<ControllerComponent>(1, &id);
    EntitySystem::MakeComps<AimToLStickComponent>(1, &id);
    EntitySystem::MakeComps<InventoryComponent>(1, &id);
    EntitySystem::MakeComps<HandComponent>(1, &id,
            args.bodyOffset,
            args.armLength
            );

    auto* ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
    if(ndc == nullptr) return;
    auto* dir = EntitySystem::GetComp<DirectionComponent>(id);
    ndc->set(DirectionData::DIR, dir->dir);
}
