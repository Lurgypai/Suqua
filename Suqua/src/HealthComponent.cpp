#include "HealthComponent.h"
#include "NetworkDataComponentDataFields.h"

HealthComponent::HealthComponent(EntityId id_) : id{ id_ }
{
	if (id != 0) {
		if (!EntitySystem::Contains<NetworkDataComponent>() || EntitySystem::GetComp<NetworkDataComponent>(id) == nullptr) {
			EntitySystem::MakeComps<NetworkDataComponent>(1, &id);
		}

		auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
		ndc->set<int32_t>(HealthData::HEALTH, 0);
		health = &ndc->get<int32_t>(HealthData::HEALTH);
	}
}

int32_t HealthComponent::getHealth() const {
	return *health;
}

void HealthComponent::setHealth(std::int32_t health_) {
	*health = health_;
}

void HealthComponent::damage(std::int32_t amount) {
	if (amount > *health) {
		*health = 0;
		return;
	}

	*health -= amount;
}
