#include "RespawnSystem.h"
#include "SpawnSystem.h"
#include "../PlayerLC.h"

RespawnSystem::RespawnSystem(SpawnSystem* spawns_) :
	spawns{spawns_}
{}

void RespawnSystem::updateAll() {
	if (EntitySystem::Contains<RespawnComponent>()) {
		for (auto& respawn : EntitySystem::GetPool<RespawnComponent>()) {
			PlayerLC* player = EntitySystem::GetComp<PlayerLC>(respawn.getId());
			if (player->shouldRespawn()) {
				respawn.loadSpawnList(*spawns);
			}
		}
	}
}