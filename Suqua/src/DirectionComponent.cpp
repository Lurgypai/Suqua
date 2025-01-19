#include "DirectionComponent.h"
#include "NetworkDataComponent.h"
#include "NetworkDataComponentDataFields.h"

#include "AngleUtil.h"
#include <stdexcept>

using namespace DirectionData;

DirectionComponent::DirectionComponent(EntityId id_) :
	id{id_},
	dir{nullptr},
	cardinalDir{ 0 },
	isLocked_{false}
{
    auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
    if(ndc == nullptr) throw std::runtime_error{"DirectionData: NetworkDataComponent was not initialized"};
    ndc->set<float>(DIR, 180.0f);
    dir = &ndc->get<float>(DIR);
}

float DirectionComponent::getDir() const {
	return *dir;
}

void DirectionComponent::setDir(float newDir) {
	if (isLocked_) return;
	*dir = newDir;

	cardinalDir = 3;
	int currCharacterDirDeg = degrees(*dir) - 45;
	if (currCharacterDirDeg < 0) currCharacterDirDeg += 360;
	if (currCharacterDirDeg >= 0 && currCharacterDirDeg <= 90) cardinalDir = 0;
	else if (currCharacterDirDeg > 90 && currCharacterDirDeg < 180) cardinalDir = 1;
	else if (currCharacterDirDeg >= 180 && currCharacterDirDeg <= 270) cardinalDir = 2;
}

int DirectionComponent::getCardinalDir() const {
	return cardinalDir;
}

bool DirectionComponent::isLocked()
{
	return isLocked_;
}

void DirectionComponent::setLocked(bool locked)
{
	isLocked_ = locked;
}
