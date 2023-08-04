#include "CombatSystem.h"
#include "EntitySystem.h"
#include "HurtboxComponent.h"
#include "HitboxComponent.h"
#include "NetworkOwnerComponent.h"
#include "TeamComponent.h"
#include "EntityBaseComponent.h"
#include "HealthComponent.h"
#include "DamageComponent.h"
#include "PhysicsComponent.h"
#include <iostream>

static inline void damageEntity(EntityId cause, EntityId receiver) {
	auto ourHealthComp = EntitySystem::GetComp<HealthComponent>(receiver);
	auto otherDamageComp = EntitySystem::GetComp<DamageComponent>(cause);
	ourHealthComp->damage(otherDamageComp->getDamage());

	auto targetPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(receiver);
	targetPhysicsComp->setVel(Vec2f{ 0, 0 });
}

void CombatSystem::checkClientCollisions(Host* host) {

	if (!EntitySystem::Contains<NetworkOwnerComponent>() || !EntitySystem::Contains<HitboxComponent>())
		return;


	for (auto& ownerComp : EntitySystem::GetPool<NetworkOwnerComponent>()) {
		// find the things that we control
		if (ownerComp.owner == NetworkOwnerComponent::Owner::local) {
			const auto hitComp = EntitySystem::GetComp<HitboxComponent>(ownerComp.getId());
			const auto base = EntitySystem::GetComp<EntityBaseComponent>(ownerComp.getId());

			if (!base->isActive || base->isDead) continue;

			// try to hit others
			if (hitComp) {
				// find other hurtboxes and run collision
				for (const auto& otherHurtComp : EntitySystem::GetPool<HurtboxComponent>()) {

					auto otherBaseComp = EntitySystem::GetComp<EntityBaseComponent>(otherHurtComp.getId());
					if (!otherBaseComp->isActive || otherBaseComp->isDead) continue;
					auto otherHealthComp = EntitySystem::GetComp<HealthComponent>(otherHurtComp.getId());
					if (otherHealthComp->getHealth() <= 0) continue;

					const auto otherTeamComp = EntitySystem::GetComp<TeamComponent>(otherHurtComp.getId());
					if (hitComp->getTeamId() != otherTeamComp->teamId) {
						if (hitComp->hitbox.intersects(otherHurtComp.hurtbox)) {
							if (hitComp->addHitEntity(otherHurtComp.getId())) {
								// run the on hit code, tell the server that the entity got hit
								damageEntity(ownerComp.getId(), otherHurtComp.getId());
								//std::cout << "We hit something!\n";
								//std::cout << hitComp->getId() << '\n';

							}
						}
					}
				}
			}

			//check if we got hit
			auto ourHealthComp = EntitySystem::GetComp<HealthComponent>(ownerComp.getId());
			if (ourHealthComp != nullptr &&  ourHealthComp->getHealth() <= 0) continue;

			const auto hurtComp = EntitySystem::GetComp<HurtboxComponent>(ownerComp.getId());
			if (hurtComp) {
				//if we can get hurt, we have to have a team
				const auto teamComp = EntitySystem::GetComp<TeamComponent>(ownerComp.getId());
				for (auto& otherHitComp : EntitySystem::GetPool<HitboxComponent>()) {
					const auto otherBaseComp = EntitySystem::GetComp<EntityBaseComponent>(otherHitComp.getId());
					if (!otherBaseComp->isActive) continue;

					if (teamComp->teamId != otherHitComp.getTeamId()) {
						if (otherHitComp.hitbox.intersects(hurtComp->hurtbox)) {
							if (otherHitComp.addHitEntity(hurtComp->getId())) {
								//std::cout << "We got hit >:(\n";
								// remove health
								damageEntity(otherHitComp.getId(), ownerComp.getId());
							}
						}
					}
				}
			}
		}
	}
}
