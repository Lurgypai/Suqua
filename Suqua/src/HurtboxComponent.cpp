#include "HurtboxComponent.h"
#include "PositionComponent.h"

HurtboxComponent::HurtboxComponent(EntityId id_) :
	id{ id_ },
	hurtbox{},
	offset{}
{}

EntityId HurtboxComponent::getId() const {
	return id;
}

void HurtboxComponent::update() {
	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
	hurtbox.pos = posComp->getPos() + offset;
}