#pragma once
#include "ComponentMacros.h"
#include "Vec2.h"

class RespawnComponent {
	CompMembers(RespawnComponent);

public:
	void update();
	void respawn();
    const int getTick() const;
    const int getRespawnDelay() const;
	
	Vec2f spawnPos;
private:
    uint32_t* tick;
	int respawnDelay;
};
