#include "HealthComponent.h"
#include "EntityBaseComponent.h"
#include "NetworkDataComponentDataFields.h"
#include <stdexcept>

HealthComponent::HealthComponent(EntityId id_, int32_t health_) : id{ id_ }
{
    auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
    if(ndc == nullptr) throw std::runtime_error{"HealthComponent: NetworkDataComponent was not initialized"};
    ndc->set<int32_t>(HealthData::HEALTH, health_);
    health = &ndc->get<int32_t>(HealthData::HEALTH);
}

int32_t HealthComponent::getHealth() const {
	return *health;
}

void HealthComponent::setHealth(std::int32_t health_) {
	*health = health_;
}

void HealthComponent::damage(std::int32_t amount) {
	if (amount >= *health) {
		*health = 0;
        auto base = EntitySystem::GetComp<EntityBaseComponent>(id);
        base->isActive = false;
		return;
	}

	*health -= amount;
}
