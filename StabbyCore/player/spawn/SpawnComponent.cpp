#include "SpawnComponent.h"
#include "RandomUtil.h"
#include <PhysicsComponent.h>
#include <PositionComponent.h>

SpawnComponent::SpawnComponent(EntityId id_) :
	id{id_}
{}

const EntityId SpawnComponent::getId() const {
	return id;
}

void SpawnComponent::generateSpawns() {
	for (int i = 0; i != 5; ++i) {
		const int maxAttempts = -1;
		for (int i = 0; i != maxAttempts; ++i) {
			Vec2f spawnPos{};
			spawnPos.x = randFloat(spawnZone.pos.x, spawnZone.pos.x + spawnZone.res.x);
			spawnPos.y = spawnZone.pos.y + spawnZone.res.y;
			bool spawnable{ false };
			for (auto& physics : EntitySystem::GetPool<PhysicsComponent>()) {
				if (physics.collideable) {
					const AABB& collider = physics.getCollider();
					const Vec2f& colliderPos = EntitySystem::GetComp<PositionComponent>(physics.getId())->pos;
					if (spawnPos.x > colliderPos.x && spawnPos.x < colliderPos.x + collider.res.x) {
						if (!spawnable || (colliderPos.y < spawnPos.y && colliderPos.y > spawnZone.pos.y)) {
							spawnPos.y = colliderPos.y;
							spawnable = true;
						}
					}
				}
			}
			if (spawnable) {
				spawnPoints.emplace_back(spawnPos);
				break;
			}
		}
	}
}

Vec2f SpawnComponent::findSpawnPos() {
	return spawnPoints[randInt(0, spawnPoints.size() - 1)];
}

const AABB& SpawnComponent::getSpawnZone() const {
	return spawnZone;
}

bool SpawnComponent::isDefault() {
	return defaultSpawnZone;
}

unsigned int SpawnComponent::getTeamId() const {
	return owningTeam;
}
