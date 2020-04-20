
#include "ServerPlayerSystem.h"
#include "EntitySystem.h"
#include "DebugFIO.h"

#include "../player/ServerPlayerComponent.h"

void ServerPlayerSystem::updatePlayers(PlayerManager & players, Time_t gameTime, const Stage & stage, SpawnSystem& spawns) {
	if (EntitySystem::Contains<ServerPlayerComponent>()) {
		for (auto& onlinePlayer : EntitySystem::GetPool<ServerPlayerComponent>()) {
			EntityId id = onlinePlayer.getId();
			onlinePlayer.readCommand(gameTime);
			players.update(id, CLIENT_TIME_STEP, stage, spawns);
		}
	}
	//get the controller state for each player
	//update with that state
}

void ServerPlayerSystem::tickPlayerTimes() {
	if (EntitySystem::Contains<ServerPlayerComponent>()) {
		for (auto& player : EntitySystem::GetPool<ServerPlayerComponent>()) {
			player.tickClientTime();
		}
	}
}
