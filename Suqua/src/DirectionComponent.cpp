#include "DirectionComponent.h"
#include "NetworkDataComponent.h"
#include "DirectionData.h"

DirectionComponent::DirectionComponent(EntityId id_) :
	id{id_},
	dir{nullptr}
{
	if (id != 0) {
		if (!EntitySystem::Contains<NetworkDataComponent>() || !EntitySystem::GetComp<NetworkDataComponent>(id)) {
			EntitySystem::MakeComps<NetworkDataComponent>(1, &id);
		}

		auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
		ndc->set<float>(DIR, 0.0f);
		dir = &ndc->get<float>(DIR);
	}
}

float DirectionComponent::getDir() const {
	return *dir;
}

void DirectionComponent::setDir(float newDir) {
	*dir = newDir;
}

EntityId DirectionComponent::getId() const {
	return id;
}
