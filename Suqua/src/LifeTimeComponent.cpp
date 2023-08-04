#include "LifeTimeComponent.h"
#include "NetworkDataComponentDataFields.h"
#include "EntityBaseComponent.h"

LifeTimeComponent::LifeTimeComponent(EntityId id_) : id{ id_ } {
	if (id != 0) {
		if (!EntitySystem::Contains<NetworkDataComponent>() || !EntitySystem::GetComp<NetworkDataComponent>(id)) {
			EntitySystem::MakeComps<NetworkDataComponent>(1, &id);
		}

		auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
		ndc->set<uint32_t>(LifeTimeData::LIFE_TIME, 0);
		remainingLife = &ndc->get<std::uint32_t>(LifeTimeData::LIFE_TIME);
	}
};

std::uint32_t LifeTimeComponent::getRemainingLife() const {
	return *remainingLife;
}

void LifeTimeComponent::setRemainingLife(std::uint32_t remainingLife_) {
	*remainingLife = remainingLife_;
}

void LifeTimeComponent::update() {
	--(*remainingLife);
	if (*remainingLife <= 0) {
		auto base = EntitySystem::GetComp<EntityBaseComponent>(id);
		base->isDead = true;
	}
}