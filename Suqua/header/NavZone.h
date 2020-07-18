#pragma once
#include <vector>

#include "AABB.h"

struct NavZone {

	AABB area;
	std::string typeTag;
	std::vector<unsigned int> adjacentZones;
	unsigned int id; //unique id, to easily identify zone
	bool rightBlocked;
	bool leftBlocked;
};