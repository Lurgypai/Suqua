#pragma once

#include "network.h"
#include "player.h"

#include "stage.h"

class ServerPlayerSystem {
public:
	void updatePlayers(PlayerManager & players, Time_t gameTime, const Stage & stage, SpawnSystem& spawns);
	void tickPlayerTimes();

};
