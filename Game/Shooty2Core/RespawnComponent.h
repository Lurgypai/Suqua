#pragma once
#include "ComponentMacros.h"
#include "Vec2.h"

class RespawnComponent {
	CompMembers(RespawnComponent);

public:
    RespawnComponent(EntityId id_,
            const Vec2f& spawnPos_,
            int spawnDelay_ = 120);
	void update();
	void respawn();
    const int getTick() const;
    const int getRespawnDelay() const;
	
	Vec2f spawnPos;
private:
    int32_t* tick;
	int respawnDelay;
};
