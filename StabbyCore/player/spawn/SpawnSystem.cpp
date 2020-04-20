#include "SpawnSystem.h"

EntityId SpawnSystem::addSpawnZone(const AABB& zone, bool defaultZone_, unsigned int teamId) {
	EntityId id;
	EntitySystem::GenEntities(1, &id);
	EntitySystem::MakeComps<SpawnComponent>(1, &id);
	SpawnComponent* spawn = EntitySystem::GetComp<SpawnComponent>(id);
	spawn->spawnZone = zone;
	spawn->defaultSpawnZone = defaultZone_;
	spawn->owningTeam = teamId;
	spawn->generateSpawns();
	return id;
}

bool SpawnSystem::teamHasSpawns(unsigned int teamId) const {
	for (auto& spawn : EntitySystem::GetPool<SpawnComponent>()) {
		if (spawn.owningTeam = teamId)
			return true;
	}
	return false;
}

void SpawnSystem::assignTeam(EntityId spawnZone, unsigned int teamId) {
	SpawnComponent* spawn = EntitySystem::GetComp<SpawnComponent>(spawnZone);
	spawn->owningTeam = teamId;
}

void SpawnSystem::clearTeamsAssigns() {
	for (auto& spawn : EntitySystem::GetPool<SpawnComponent>()) {
		spawn.owningTeam = 0;
	}
}
