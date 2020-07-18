#pragma once
#include "Vec2.h"
#include "NavZone.h"
#include <vector>
#include <list>
#include <unordered_map>
class NavMesh {
public:
	NavMesh(float zoneHeight_, float ignoreGapSize_);
	void addPhysicsMesh();
	std::list<unsigned int> getPath(unsigned int startZoneId, unsigned int endZoneId);
	const NavZone& getZone(unsigned int id) const;
	const NavZone* getZone(Vec2f pos) const;
	const std::unordered_map<unsigned int, NavZone> getZones() const;
protected:
	unsigned int zoneId;
	//the height of each generated zone
	float zoneHeight;
	//the smallest gap that can be ignored (walked across)
	float ignoreGapSize;
	std::unordered_map<unsigned int, NavZone> zones;
};