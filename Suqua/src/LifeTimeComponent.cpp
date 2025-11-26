#include "LifeTimeComponent.h"
#include "NetworkDataComponentDataFields.h"
#include "EntityBaseComponent.h"
#include <stdexcept>

LifeTimeComponent::LifeTimeComponent(EntityId id_, int32_t remainingLife_) :
    id{ id_ },
    remainingLife{remainingLife_}
{
};

std::int32_t LifeTimeComponent::getRemainingLife() const {
	return remainingLife;
}

void LifeTimeComponent::setRemainingLife(std::int32_t remainingLife_) {
	remainingLife = remainingLife_;
}

void LifeTimeComponent::update() {
	--remainingLife;
	if (remainingLife <= 0) {
		auto base = EntitySystem::GetComp<EntityBaseComponent>(id);
		base->isDead = true;
	}
}
