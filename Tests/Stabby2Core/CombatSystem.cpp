#include "CombatSystem.h"
#include "PlayerComponent.h"
#include "PhysicsComponent.h"

void CombatSystem::update() {
	if (EntitySystem::Contains<PlayerComponent>()) {
		for (const auto& player : EntitySystem::GetPool<PlayerComponent>()) {
			auto attack = player.getCurrAttack();
			if (attack) {
				for (const auto& physics : EntitySystem::GetPool<PhysicsComponent>()) {
					if (physics.getId() != player.getId() &&
						attack->intersects(physics.getCollider())) {
						std::cout << "Ouch!\n";
					}
				}
			}
		}
	}
}
