#include "EntitySystem.h"
#include "PositionComponent.h"
#include "PhysicsComponent.h"

#include "EntitySpawnSystem.h"
#include "EntitySpawnComponent.h"
#include "ActiveEntityZoneComponent.h"
#include "EntityGenerator.h"


using AEZComp = ActiveEntityZoneComponent;

void EntitySpawnSystem::update(Scene& scene) {
    for(auto& entityZoneComp : EntitySystem::GetPool<AEZComp>()) {
        for(auto& entitySpawnComp : EntitySystem::GetPool<EntitySpawnComponent>()) {
            EntityId zoneId = entityZoneComp.getId();
            EntityId spawnId = entitySpawnComp.getId();

            if(zoneId == spawnId) continue;

            auto posComp = EntitySystem::GetComp<PositionComponent>(spawnId);
            Vec2f pos = posComp->getPos();

            auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(spawnId);
            if(physicsComp) pos = physicsComp->position();

            if(entityZoneComp.isOutsideInactiveBox(pos)) {
                entityZoneComp.removeTriggered(spawnId);
                continue;
            }

            // isn't already within active box
            if(entityZoneComp.hasTriggered(spawnId)) continue;

            if(!entityZoneComp.isWithinActiveBox(pos)) continue;

            EntityGenerator::SpawnEntity(scene, entitySpawnComp.spawnTag, pos);
        }
    }
}
