#pragma once
#include "SpawnComponent.h"
#include <unordered_map>
#include <set>

class SpawnSystem {
public:
	EntityId addSpawnZone(const AABB& zone, bool defaultZone_, unsigned int teamId);
	bool teamHasSpawns(unsigned int teamId) const;
	void assignTeam(EntityId spawnZone, unsigned int teamId);
	//remove the assignment to teams
	void clearTeamsAssigns();
};