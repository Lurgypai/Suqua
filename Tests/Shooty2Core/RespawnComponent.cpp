#include "RespawnComponent.h"
#include "HealthComponent.h"
#include "PhysicsComponent.h"
#include "CharacterGFXComponent.h"
#include "EntityBaseComponent.h"

RespawnComponent::RespawnComponent(EntityId id_) :
	id{ id_ },
	tick{ 0 },
	respawnDelay{ 120 },
	spawnPos{ 0, 0 }
{}

void RespawnComponent::update() {
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
	physicsComp->teleport(spawnPos);
	physicsComp->setVel({ 0, 0 });

	auto healthComponent = EntitySystem::GetComp<HealthComponent>(id);
	healthComponent->setHealth(100);
}
