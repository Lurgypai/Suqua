#include "ClimbableComponent.h"
#include "NetworkDataComponent.h"

ClimbableComponent::ClimbableComponent(EntityId id_) :
	id{id_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<NetworkDataComponent>() || EntitySystem::GetComp<NetworkDataComponent>(id) == nullptr) {
			EntitySystem::MakeComps<NetworkDataComponent>(1, &id);
		}
	}
}

EntityId ClimbableComponent::getId() const {
	return id;
}
