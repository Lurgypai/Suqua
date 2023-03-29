#include "ParentComponent.h"
#include "PositionComponent.h"

ParentComponent::ParentComponent(EntityId id_) :
	id{ id_ },
	parentId{ 0 }
{
	if (id != 0) {
		if (!EntitySystem::Contains<PositionComponent>() || !EntitySystem::GetComp<PositionComponent>(id)) {
			EntitySystem::MakeComps<PositionComponent>(1, &id);
		}
	}
}

EntityId ParentComponent::getId() const {
	return id;
}

void ParentComponent::update() {
	auto parentPosComp = EntitySystem::GetComp<PositionComponent>(parentId);
	if (!parentPosComp) return;

	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
	posComp->setPos(parentPosComp->getPos() + offset);
}