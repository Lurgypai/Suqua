#pragma once
#include "EntitySystem.h"
#include "Controller.h"
#include "../combat/WeaponManager.h"
#include "../stage/Stage.h"
#include "PlayerLC.h"

class PlayerManager {
public:
	EntityId makePlayer(const WeaponManager & weapons);

	void updateAll(double timeDelta, const Stage & stage, SpawnSystem& spawns);
	void update(EntityId player, double timeDelta, const Stage& stage, SpawnSystem& spawns);

private:
	void updatePlayer(double timeDelta, PlayerLC& player, const Stage& stage, SpawnSystem& spawns);
};