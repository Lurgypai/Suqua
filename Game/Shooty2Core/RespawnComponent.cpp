#include "RespawnComponent.h"
#include "HealthComponent.h"
#include "PhysicsComponent.h"
#include "EntityBaseComponent.h"
#include "NetworkDataComponent.h"
#include "Shooty2NetworkDataFields.h"

RespawnComponent::RespawnComponent(EntityId id_,
        const Vec2f& spawnPos_,
        int spawnDelay_) :
	id{ id_ },
	tick{ 0 },
	respawnDelay{ spawnDelay_ },
	spawnPos{ spawnPos_ }
{
}

void RespawnComponent::update() {
	auto healthComp = EntitySystem::GetComp<HealthComponent>(id);
	if (healthComp->getHealth() <= 0) {
		if (tick < respawnDelay) {
			auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
			physicsComp->vel ={ 0, 0 };
			++(tick);
		}
		else if (tick == respawnDelay) {
			tick = 0;
			auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(id);
			baseComp->isActive = true;
			respawn();
		}
	}
}

void RespawnComponent::respawn() {
	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	physicsComp->teleport(spawnPos);
	physicsComp->vel = { 0, 0 };

	auto healthComponent = EntitySystem::GetComp<HealthComponent>(id);
	healthComponent->setHealth(100);
}

const int RespawnComponent::getTick() const {
    return tick;
}

const int RespawnComponent::getRespawnDelay() const {
    return respawnDelay;
}
