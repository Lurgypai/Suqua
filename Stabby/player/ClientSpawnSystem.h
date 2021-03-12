#pragma once
#include "EntitySystem.h"
#include "player.h"

class ClientSpawnSystem {
public:
	void updatePlayerSpawn(EntityId player);
	void updateAllPlayerSpawns();
	void setSpawns(SpawnSystem* spawns_);
	void setSpawnSystem(SpawnSystem* spawns);
private:
	SpawnSystem* spawns;
};