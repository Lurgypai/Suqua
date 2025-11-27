#include <print>

#include "HealthComponent.h"
#include "EntityBaseComponent.h"
#include "NetworkDataComponentDataFields.h"
#include <stdexcept>

HealthComponent::HealthComponent(EntityId id_, int32_t health_, DeathCallback deathCallback_) :
    id{ id_ },
    health{ health_ },
    deathCallback{deathCallback_}
{ }

int32_t HealthComponent::getHealth() const {
	return health;
}

void HealthComponent::setHealth(std::int32_t health_) {
	health = health_;
}

void HealthComponent::damage(std::int32_t amount) {
	if (amount >= health) {
		health = 0;
        auto base = EntitySystem::GetComp<EntityBaseComponent>(id);
        base->isDead = true;
        if(deathCallback) deathCallback();
		return;
	}

	health -= amount;
}
