#pragma once
#include "RespawnComponent.h"

class RespawnSystem {
public:
	RespawnSystem(SpawnSystem* spawns_);
	void updateAll();
private:
	SpawnSystem* spawns;
};