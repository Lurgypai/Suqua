#pragma once
#include "Vec2.h"
#include "NavTypes.h"
#include "NavZone.h"
#include "NavPath.h"
#include <vector>
#include <list>
#include <unordered_map>

class NavMesh {
public:
	NavMesh(float zoneHeight_, float ignoreGapSize_);
	void addPhysicsMesh();
	NavPath getPath(NavZoneId startNavZoneId, NavZoneId endNavZoneId);
	const NavZone& getZone(NavZoneId id) const;
	const NavZone* getZone(Vec2f pos) const;
	const std::unordered_map<NavZoneId, NavZone> getZones() const;
protected:
	unsigned int zoneId;
	//the height of each generated zone
	float zoneHeight;
	//the smallest gap that can be ignored (walked across)
	float ignoreGapSize;
	std::unordered_map<NavZoneId, NavZone> zones;
};