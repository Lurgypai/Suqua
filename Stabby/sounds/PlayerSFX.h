#pragma once
#include "EntitySystem.h"
#include "../player/PlayerStateComponent.h"

class PlayerSFX {
public:
	PlayerSFX(EntityId id_ = 0);
	EntityId getId() const;

	void update();
private:
	int32_t prevHealth;
	EntityId id;
};