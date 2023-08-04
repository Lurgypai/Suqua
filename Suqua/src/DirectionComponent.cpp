#include "DirectionComponent.h"
#include "NetworkDataComponent.h"
#include "NetworkDataComponentDataFields.h"

using namespace DirectionData;

DirectionComponent::DirectionComponent(EntityId id_) :
	id{id_},
	dir{nullptr},
	isLocked_{false}
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
	if (isLocked_) return;
	*dir = newDir;
}

bool DirectionComponent::isLocked()
{
	return isLocked_;
}

void DirectionComponent::setLocked(bool locked)
{
	isLocked_ = locked;
}

EntityId DirectionComponent::getId() const {
	return id;
}
