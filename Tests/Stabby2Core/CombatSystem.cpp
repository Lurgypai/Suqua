#include "CombatSystem.h"
#include "PlayerComponent.h"
#include "ZombieComponent.h"
#include "PhysicsComponent.h"

#include <cmath>

void CombatSystem::update() {
	if (EntitySystem::Contains<PlayerComponent>() && EntitySystem::Contains<ZombieComponent>()) {
		for (auto& player : EntitySystem::GetPool<PlayerComponent>()) {
			if (player.hitboxChanged()) player.clearMarked();

			for (auto& zombie : EntitySystem::GetPool<ZombieComponent>()) {
				if (zombie.hitboxChanged()) zombie.clearMarked();


				auto* zombiePhysics = EntitySystem::GetComp<PhysicsComponent>(zombie.getId());
				auto* playerPhysics = EntitySystem::GetComp<PhysicsComponent>(player.getId());

				bool hitZombie = false;
				bool hitPlayer = false;
				//player attack zombie
				auto attack = player.getCurrAttack();
				if (attack) {
					if (!player.hasHit(zombie.getId()) &&
						attack->intersects(zombiePhysics->getCollider())) {
						hitZombie = true;
					}
				}

				//zombie attack player
				if (zombie.getState() == ZombieComponent::ZombieState::attacking) {
					if(!zombie.hasHit(player.getId()) &&
						zombiePhysics->getCollider().intersects(playerPhysics->getCollider())) {
						hitPlayer = true;
					}
				}

				//reslove
				if (hitZombie) {
					player.markHit(zombie.getId());
					zombie.damage(10);
					player.beginFreeze();
					zombie.beginFreeze();
				}
				if (hitPlayer) {
					zombie.markHit(player.getId());
					player.damage(34);
					player.beginFreeze();
					zombie.beginFreeze();
				}
			}
		}
	}
}
