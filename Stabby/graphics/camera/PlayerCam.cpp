#include <iostream>

#include "PlayerCam.h"
#include "PhysicsComponent.h"
#include "PositionComponent.h"

#include "player.h"

PlayerCam::PlayerCam(int windowWidth, int windowHeight) :
	Camera{ Vec2i{0, 0}, Vec2i{windowWidth, windowHeight}, 1 }
{
}

void PlayerCam::update(EntityId targetId) {

	Vec2f prevPos = pos;

	if (EntitySystem::Contains<PlayerLC>()) {
		auto player = EntitySystem::GetComp<PlayerLC>(targetId);
		if (player != nullptr) {
			if (!player->shouldRespawn()) {
				if (EntitySystem::Contains<PhysicsComponent>()) {
					PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(targetId);
					if (physics != nullptr) {
						center(physics->center());
					}
					else {
						PositionComponent* position = EntitySystem::GetComp<PositionComponent>(targetId);
						if (position != nullptr) {
							center(position->pos);
						}
					}
				}
			}
			else {
				RespawnComponent* respawn = EntitySystem::GetComp<RespawnComponent>(targetId);
				SpawnComponent* targetSpawn = EntitySystem::GetComp<SpawnComponent>(respawn->getCurrentSpawn());
				if (targetSpawn != nullptr) {
					center(targetSpawn->getSpawnZone().center());
				}
			}
		}
	}

	Vec2f newPos = pos;

	//bigger number here means move slower
	pos = prevPos + ((newPos - prevPos) / 3);
}
