#include "HealthComponent.h"
#include "NetworkDataComponentDataFields.h"

HealthComponent::HealthComponent(EntityId id_) : id{ id_ }
{
	if (id != 0) {
		if (!EntitySystem::Contains<NetworkDataComponent>() || EntitySystem::GetComp<NetworkDataComponent>(id) == nullptr) {
			EntitySystem::MakeComps<NetworkDataComponent>(1, &id);
		}

		auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
		ndc->set<uint32_t>(HealthData::HEALTH, 0);
		health = &ndc->get<uint32_t>(HealthData::HEALTH);
	}
}

uint32_t HealthComponent::getHealth() const {
	return *health;
}

void HealthComponent::setHealth(std::uint32_t health_) {
	*health = health_;
}

void HealthComponent::damage(std::uint32_t amount) {
	if (amount > *health) {
		*health = 0;
		return;
	}

	*health -= amount;
}