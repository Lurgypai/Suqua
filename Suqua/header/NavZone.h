#pragma once
#include <vector>

#include "AABB.h"
#include "NavTypes.h"

struct NavZone {

	AABB area;
	std::string typeTag;
	std::vector<unsigned int> adjacentZones;
	NavZoneId id; //unique id, to easily identify zone
	bool rightBlocked;
	bool leftBlocked;
};