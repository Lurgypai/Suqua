#pragma once
#include "ComponentMacros.h"
#include "Vec2.h"

class RespawnComponent {
	CompMembers(RespawnComponent);

public:
	void update();
	void respawn();
    EntityId spawnZoneId;
private:
	int tick;
	int respawnDelay;
};
