#include "ClientSpawnSystem.h"
#include "player.h"
#include "network.h"

void ClientSpawnSystem::updatePlayerSpawn(EntityId player) {
	PlayerLC* playerComp = EntitySystem::GetComp<PlayerLC>(player);
	if (playerComp->shouldRespawn()) {
		RespawnComponent* respawn = EntitySystem::GetComp<RespawnComponent>(player);
		switch (respawn->getState())
		{
		case none:
		case unselected:
			respawn->beginSelecting();
			break;
		case selecting:
			respawn->updateSpawnList(*spawns);
			respawn->searchForSpawn();
			break;
		case selected: {
			SpawnComponent* spawnZone = EntitySystem::GetComp<SpawnComponent>(respawn->getCurrentSpawn());
			playerComp->respawn(spawnZone->findSpawnPos());
			respawn->reset();
			
		}
			break;
		default:
			break;
		}
	}
}

void ClientSpawnSystem::updateAllPlayerSpawns() {
	for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
		updatePlayerSpawn(player.getId());
	}
}

void ClientSpawnSystem::setSpawns(SpawnSystem* spawns_) {
	spawns = spawns_;
}
