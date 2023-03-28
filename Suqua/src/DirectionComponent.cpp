#include "DirectionComponent.h"
#include "NetworkDataComponent.h"
#include "DirectionData.h"

DirectionComponent::DirectionComponent(EntityId id_) :
	id{id_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<NetworkDataComponent>() || !EntitySystem::GetComp<NetworkDataComponent>(id)) {
			EntitySystem::MakeComps<NetworkDataComponent>(1, &id);
		}

		auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
		ndc->set<float>(DIR, 0.0f);
	}
}

float DirectionComponent::getDir() const {
	auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
	return ndc->get<float>(DIR);
}

void DirectionComponent::setDir(float newDir) {
	auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
	ndc->get<float>(DIR) = newDir;
}

EntityId DirectionComponent::getId() const {
	return id;
}
