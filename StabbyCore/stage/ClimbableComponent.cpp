#include "ClimbableComponent.h"
#include "PositionComponent.h"

ClimbableComponent::ClimbableComponent(EntityId id_) :
	id{id_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<PositionComponent>() || EntitySystem::GetComp<PositionComponent>(id) == nullptr) {
			EntitySystem::MakeComps<PositionComponent>(1, &id);
		}
	}
}

EntityId ClimbableComponent::getId() const {
	return id;
}
