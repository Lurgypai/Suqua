#include "EntityAddPhysics.h"
#include "PositionComponent.h"
#include "PhysicsComponent.h"
#include "NetworkDataComponent.h"
#include "NetworkDataComponentDataFields.h"

void EntityAddPhysics(EntityId id, const EntityAddPhysicsArgs& args) {
    EntitySystem::MakeComps<PhysicsComponent>(1, &id,
            args.pos,
            args.res,
            args.collideable,
            args.collideableWith
            );
    auto* phys = EntitySystem::GetComp<PhysicsComponent>(id);
    if(args.centered) phys->center(args.pos);

    auto* ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
    if(ndc == nullptr) return;
    auto* pos = EntitySystem::GetComp<PositionComponent>(id);
    ndc->set(PositionData::X, pos->pos.x);
    ndc->set(PositionData::Y, pos->pos.y);
    ndc->set(PhysicsData::XVEL, phys->vel.x);
    ndc->set(PhysicsData::YVEL, phys->vel.y);
}
