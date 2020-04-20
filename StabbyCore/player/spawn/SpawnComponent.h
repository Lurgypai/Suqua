#pragma once
#include "EntitySystem.h"
#include <vector>
#include <AABB.h>

class SpawnComponent {
	friend class SpawnSystem;
public:
	SpawnComponent(EntityId id_ = 0);
	const EntityId getId() const;

	void generateSpawns();
	Vec2f findSpawnPos();
	const AABB& getSpawnZone() const;
	bool isDefault();
	unsigned int getTeamId() const;

private:
	EntityId id;
	AABB spawnZone;
	bool defaultSpawnZone;
	unsigned int owningTeam;
	std::vector<Vec2f> spawnPoints;
};