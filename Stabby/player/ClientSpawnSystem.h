#pragma once
#include "EntitySystem.h"
#include "../client/Client.h"
#include "player.h"

class ClientSpawnSystem {
public:
	void updatePlayerSpawn(EntityId player);
	void updateAllPlayerSpawns();
	void setClient(Client* client_);
	void setSpawns(SpawnSystem* spawns_);
	void setSpawnSystem(SpawnSystem* spawns);
private:
	Client* client;
	SpawnSystem* spawns;
};