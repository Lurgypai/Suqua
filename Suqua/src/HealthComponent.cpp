#include <print>

#include "HealthComponent.h"
#include "EntityBaseComponent.h"
#include "NetworkDataComponentDataFields.h"
#include <stdexcept>

HealthComponent::HealthComponent(EntityId id_, int32_t health_) :
    id{ id_ },
    health{ health_ }
{ }

int32_t HealthComponent::getHealth() const {
	return health;
}

void HealthComponent::setHealth(std::int32_t health_) {
	health = health_;
}

void HealthComponent::damage(std::int32_t amount) {
	if (amount >= health) {

        /* DEBUG */
        static int DEBUG_DEATH_COUNT{0};
        std::println("Death count: {}", ++DEBUG_DEATH_COUNT);
        /* END DEBUG */

		health = 0;
        auto base = EntitySystem::GetComp<EntityBaseComponent>(id);
        base->isActive = false;
		return;
	}

	health -= amount;
}
