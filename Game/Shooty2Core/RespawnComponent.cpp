#include "RespawnComponent.h"
#include "HealthComponent.h"
#include "PhysicsComponent.h"
#include "EntityBaseComponent.h"
#include "NetworkDataComponent.h"
#include "Shooty2NetworkDataFields.h"

RespawnComponent::RespawnComponent(EntityId id_) :
	id{ id_ },
	tick{ nullptr },
	respawnDelay{ 120 },
	spawnPos{ 0, 0 }
{
    if(id != 0) {
        auto* data = EntitySystem::GetComp<NetworkDataComponent>(id);

        data->set<std::uint32_t>(RespawnData::RESPAWN_TICK, 0);
        tick = &data->get<std::uint32_t>(RespawnData::RESPAWN_TICK);
    }
}

void RespawnComponent::update() {
	auto healthComp = EntitySystem::GetComp<HealthComponent>(id);
	if (healthComp->getHealth() <= 0) {
		if (*tick < respawnDelay) {
			auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
			physicsComp->setVel({ 0, 0 });
			++(*tick);
		}
		else if (*tick == respawnDelay) {
			*tick = 0;
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

const int RespawnComponent::getTick() const {
    return *tick;
}

const int RespawnComponent::getRespawnDelay() const {
    return respawnDelay;
}
