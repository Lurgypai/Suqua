#include "ActiveEntityComponent.h"
#include "EntitySystem.h"
#include "EntityBaseComponent.h"
#include "PhysicsComponent.h"
#include "PositionComponent.h"
#include "ActiveEntityZoneComponent.h"

ActiveEntityComponent::ActiveEntityComponent(EntityId id_) :
    id{id_},
    spawnPos{},
    canReset{true}
{}

void ActiveEntityComponent::update() {
    auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
    auto* baseComp = EntitySystem::GetComp<EntityBaseComponent>(id);
    for(auto& activeZoneComp : EntitySystem::GetPool<ActiveEntityZoneComponent>()) {
        //outside, despawn
        if(baseComp->isActive &&
                activeZoneComp.isOutsideInactiveBox(physicsComp->position())) deactivate();

        //spawn is outside, enable respawning
        if(activeZoneComp.isOutsideInactiveBox(spawnPos)) canReset = true;

        //inside respawn
        if(canReset && !baseComp->isActive && activeZoneComp.isWithinActiveBox(spawnPos)) reset();
    }
}

void ActiveEntityComponent::deactivate() {
    auto* baseComp = EntitySystem::GetComp<EntityBaseComponent>(id);
    baseComp->isActive = false;
}

void ActiveEntityComponent::activate() {
    auto* baseComp = EntitySystem::GetComp<EntityBaseComponent>(id);
    baseComp->isActive = true;
}

void ActiveEntityComponent::reset() {
    auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
    physicsComp->teleport(spawnPos);
    activate();
    canReset = false;
}
