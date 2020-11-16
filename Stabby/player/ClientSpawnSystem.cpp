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
			if (!client->getConnected()) {
				SpawnComponent* spawnZone = EntitySystem::GetComp<SpawnComponent>(respawn->getCurrentSpawn());
				playerComp->respawn(spawnZone->findSpawnPos());
				respawn->reset();
			}
			else {
				RespawnRequestPacket respawnRequest;
				OnlineComponent* respawnOnline = EntitySystem::GetComp<OnlineComponent>(respawn->getCurrentSpawn());
				respawnRequest.targetRespawnComp = respawnOnline->getNetId();
				client->buffer(respawnRequest);
				respawn->reset();
			}
		}
					 break;
		default:
			break;
		}
	}
	else {
		if (client->getConnected()) {
			RespawnComponent* respawn = EntitySystem::GetComp<RespawnComponent>(player);
			respawn->reset();
		}
	}
}

void ClientSpawnSystem::updateAllPlayerSpawns() {
	for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
		updatePlayerSpawn(player.getId());
	}
}

void ClientSpawnSystem::setClient(Client* client_) {
	client = client_;
}

void ClientSpawnSystem::setSpawns(SpawnSystem* spawns_) {
	spawns = spawns_;
}
