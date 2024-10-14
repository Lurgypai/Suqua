#include "RespawnComponent.h"
#include "HealthComponent.h"
#include "PhysicsComponent.h"
#include "CharacterGFXComponent.h"
#include "EntityBaseComponent.h"
#include "RespawnZoneComponent.h"

RespawnComponent::RespawnComponent(EntityId id_) :
	id{ id_ },
	tick{ 0 },
	respawnDelay{ 120 }
{}

void RespawnComponent::update() {
    auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
    if(EntitySystem::Contains<RespawnZoneComponent>()) {
        for(auto& respawnComp : EntitySystem::GetPool<RespawnZoneComponent>()) {
            if(respawnComp.zone.contains(physicsComp->position())) spawnZoneId = respawnComp.getId();
        }
    }

	auto healthComp = EntitySystem::GetComp<HealthComponent>(id);
	if (healthComp->getHealth() <= 0) {
		if (tick < respawnDelay) {
			if (tick == 0) {
				auto gfxComp = EntitySystem::GetComp<CharacterGFXComponent>(id);
				gfxComp->playAnimation("dead", false);
			}
			auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
			physicsComp->setVel({ 0, 0 });
			++tick;
		}
		else if (tick == respawnDelay) {
			tick = 0;

			auto gfxComp = EntitySystem::GetComp<CharacterGFXComponent>(id);
			gfxComp->stopAnimation();
			auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(id);
			baseComp->isActive = true;
			respawn();
		}
	}
}

void RespawnComponent::respawn() {
	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
    auto targetRespawnComp = EntitySystem::GetComp<RespawnZoneComponent>(spawnZoneId);
    physicsComp->teleport(targetRespawnComp->zone.center());
	physicsComp->setVel({ 0, 0 });

	auto healthComponent = EntitySystem::GetComp<HealthComponent>(id);
	healthComponent->setHealth(2);
}

const int RespawnComponent::getTick() const {
    return tick;
}
