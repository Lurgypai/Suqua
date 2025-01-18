#include "LifeTimeComponent.h"
#include "NetworkDataComponentDataFields.h"
#include "EntityBaseComponent.h"

LifeTimeComponent::LifeTimeComponent(EntityId id_, int32_t remainingLife_) : id{ id_ } {
    if (!EntitySystem::Contains<NetworkDataComponent>() ||
            !EntitySystem::GetComp<NetworkDataComponent>(id)) {
        EntitySystem::MakeComps<NetworkDataComponent>(1, &id);
    }

    auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
    ndc->set<int32_t>(LifeTimeData::LIFE_TIME, remainingLife_);
    remainingLife = &ndc->get<std::int32_t>(LifeTimeData::LIFE_TIME);
};

std::int32_t LifeTimeComponent::getRemainingLife() const {
	return *remainingLife;
}

void LifeTimeComponent::setRemainingLife(std::int32_t remainingLife_) {
	*remainingLife = remainingLife_;
}

void LifeTimeComponent::update() {
	--(*remainingLife);
	if (*remainingLife <= 0) {
		auto base = EntitySystem::GetComp<EntityBaseComponent>(id);
		base->isDead = true;
	}
}
