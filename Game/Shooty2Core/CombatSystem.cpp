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

static inline void damageEntity(EntityId cause, EntityId receiver) {
	auto ourHealthComp = EntitySystem::GetComp<HealthComponent>(receiver);
	auto otherDamageComp = EntitySystem::GetComp<DamageComponent>(cause);
	ourHealthComp->damage(otherDamageComp->getDamage());

	auto targetPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(receiver);
	targetPhysicsComp->setVel(Vec2f{ 0, 0 });
}

using TeamId = TeamComponent::TeamId;

void CombatSystem::checkClientCollisions(Host* host) {

	if (!EntitySystem::Contains<NetworkOwnerComponent>() || !EntitySystem::Contains<HitboxComponent>())
		return;


	for (auto& ownerComp : EntitySystem::GetPool<NetworkOwnerComponent>()) {

		if (ownerComp.owner != NetworkOwnerComponent::Owner::local) continue;

		// find the things that we control
		const auto hitComp = EntitySystem::GetComp<HitboxComponent>(ownerComp.getId());
		const auto base = EntitySystem::GetComp<EntityBaseComponent>(ownerComp.getId());

		if (!base->isActive || base->isDead) continue;

		// try to hit others
		if (hitComp) {
			// find other hurtboxes and run collision
			for (const auto& otherHurtComp : EntitySystem::GetPool<HurtboxComponent>()) {
				if (otherHurtComp.getId() == ownerComp.getId()) continue;

				auto otherBaseComp = EntitySystem::GetComp<EntityBaseComponent>(otherHurtComp.getId());
				if (!otherBaseComp->isActive || otherBaseComp->isDead) continue;
				auto otherHealthComp = EntitySystem::GetComp<HealthComponent>(otherHurtComp.getId());
				if (otherHealthComp->getHealth() <= 0) continue;

				const auto otherTeamComp = EntitySystem::GetComp<TeamComponent>(otherHurtComp.getId());

				if (hitComp->getTeamId() != TeamId::neutral &&
                    otherTeamComp->teamId != TeamId::neutral &&
                    hitComp->getTeamId() == otherTeamComp->teamId) continue;
				if (!hitComp->hitbox.intersects(otherHurtComp.hurtbox)) continue;
				if (!hitComp->addHitEntity(otherHurtComp.getId())) continue;

				damageEntity(ownerComp.getId(), otherHurtComp.getId());
			}
		}

		//check if we got hit
		auto ourHealthComp = EntitySystem::GetComp<HealthComponent>(ownerComp.getId());
		if (ourHealthComp != nullptr &&  ourHealthComp->getHealth() <= 0) continue;

		const auto hurtComp = EntitySystem::GetComp<HurtboxComponent>(ownerComp.getId());
		if (hurtComp == nullptr) continue;

		const auto teamComp = EntitySystem::GetComp<TeamComponent>(ownerComp.getId());
		for (auto& otherHitComp : EntitySystem::GetPool<HitboxComponent>()) {
			if (otherHitComp.getId() == ownerComp.getId()) continue;

			const auto otherBaseComp = EntitySystem::GetComp<EntityBaseComponent>(otherHitComp.getId());
			if (!otherBaseComp->isActive) continue;

			const auto otherNetworkOwnerComp = EntitySystem::GetComp<NetworkOwnerComponent>(otherHitComp.getId());
			if (otherNetworkOwnerComp->owner != NetworkOwnerComponent::Owner::foreign) continue;

			if (teamComp->teamId != TeamId::neutral &&
                otherHitComp.getTeamId() != TeamId::neutral && 
                teamComp->teamId == otherHitComp.getTeamId()) continue;
			if (!otherHitComp.hitbox.intersects(hurtComp->hurtbox)) continue;
			if (!otherHitComp.addHitEntity(hurtComp->getId())) continue;

			damageEntity(otherHitComp.getId(), ownerComp.getId());
		}
	}
}
