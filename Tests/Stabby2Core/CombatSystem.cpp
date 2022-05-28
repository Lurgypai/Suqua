#include "CombatSystem.h"
#include "PlayerComponent.h"
#include "PhysicsComponent.h"

void CombatSystem::update() {
	if (EntitySystem::Contains<PlayerComponent>()) {
		for (auto& player : EntitySystem::GetPool<PlayerComponent>()) {
			auto attack = player.getCurrAttack();
			if (attack) {
				for (const auto& physics : EntitySystem::GetPool<PhysicsComponent>()) {
					if (physics.getId() != player.getId() &&
						!player.hasHit(physics.getId()) &&
						attack->intersects(physics.getCollider())) {
						auto* otherPlayer = EntitySystem::GetComp<PlayerComponent>(physics.getId());
						otherPlayer->damage(10);
						auto currHitbox = attack->getCurrHitbox();

						const auto& dir = EntitySystem::GetComp<NetworkDataComponent>(player.getId())->get<int32_t>(PlayerData::DIR);
						launchData.emplace(physics.getId(), Vec2f{ cosf(currHitbox->launchAngle) * dir, -sinf(currHitbox->launchAngle) });

						player.markHit(otherPlayer->getId());
					}
				}
			}
			if (player.hitboxChanged()) {
				player.clearMarked();
			}
		}

		for (auto player : EntitySystem::GetPool<PlayerComponent>()) {
			if (launchData.find(player.getId()) != launchData.end()) {
				player.launch(launchData[player.getId()]);
			}
		}

		launchData.clear();
	}
}
