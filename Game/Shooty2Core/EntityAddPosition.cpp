#include "EntityAddPosition.h"
#include "PositionComponent.h"
#include "NetworkDataComponent.h"
#include "NetworkDataComponentDataFields.h"

void EntityAddPosition(EntityId id, const EntityPositionArgs& args) {
    EntitySystem::MakeComps<PositionComponent>(1, &id);
    auto* pos = EntitySystem::GetComp<PositionComponent>(id);
    pos->pos = args.pos;

    auto* ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
    if(ndc != nullptr) {
        ndc->set(PositionData::X, pos->pos.x);
        ndc->set(PositionData::Y, pos->pos.y);
    }
}
