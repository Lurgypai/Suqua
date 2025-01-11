#pragma once
#include "ComponentMacros.h"

class RespawnComponent {
	CompMembers(RespawnComponent);

public:
	void update();
	void respawn();
    const int getTick() const;
    EntityId spawnZoneId;
private:
	int tick;
	int respawnDelay;
};
