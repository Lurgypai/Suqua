#include "DirectionComponent.h"
#include "NetworkDataComponent.h"
#include "NetworkDataComponentDataFields.h"

#include "AngleUtil.h"
#include <stdexcept>

using namespace DirectionData;

DirectionComponent::DirectionComponent(EntityId id_) :
	id{id_},
    dir{180.f},
	cardinalDir{ 0 },
	isLocked_{false}
{}

float DirectionComponent::getDir() const {
	return dir;
}

void DirectionComponent::setDir(float newDir) {
	if (isLocked_) return;
	dir = newDir;

	cardinalDir = 3;
	int currCharacterDirDeg = degrees(dir) - 45;
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
