#include <print>

#include "CombatSystem.h"
#include "EntitySystem.h"
#include "HurtboxComponent.h"
#include "HitboxComponent.h"
#include "Shooty2Packet.h"
#include "TeamComponent.h"
#include "EntityBaseComponent.h"
#include "HealthComponent.h"
#include "DamageComponent.h"
#include "PhysicsComponent.h"
#include "Packet.h"
#include "NetworkDataComponent.h"
#include "NetworkDataComponentDataFields.h"

static inline void damageEntity(EntityId cause, EntityId receiver, ByteStream& packet) {
	auto otherDamageComp = EntitySystem::GetComp<DamageComponent>(cause);

    auto ndc = EntitySystem::GetComp<NetworkDataComponent>(receiver);
    auto damage = otherDamageComp->getDamage();
    // foreign entity hit, send packet
    if(ndc->owner == NetworkDataComponent::Owner::foreign) {
        packet << ndc->getUUID();
        packet << damage;

    }
    auto ourHealthComp = EntitySystem::GetComp<HealthComponent>(receiver);
    auto health = ourHealthComp->health;
    ourHealthComp->damage(damage);
}

using TeamId = TeamComponent::TeamId;

void CombatSystem::checkClientCollisions(Host* host) {

	if (!EntitySystem::Contains<HitboxComponent>())
		return;


    ByteStream damagePacket;
    damagePacket << Shooty2Packet::Damage;

	for (auto& ndc : EntitySystem::GetPool<NetworkDataComponent>()) {

		// find the things that we control
		if (ndc.owner != NetworkDataComponent::Owner::local_only &&
                ndc.owner != NetworkDataComponent::Owner::local_shared) continue;

		const auto base = EntitySystem::GetComp<EntityBaseComponent>(ndc.getId());
		if (!base->isActive || base->isDead) continue;

		const auto hitComp = EntitySystem::GetComp<HitboxComponent>(ndc.getId());
		// try to hit others
		if (hitComp != nullptr) {
			// find other hurtboxes and run collision
			for (const auto& otherHurtComp : EntitySystem::GetPool<HurtboxComponent>()) {
				if (otherHurtComp.getId() == ndc.getId()) continue;

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

				damageEntity(ndc.getId(), otherHurtComp.getId(), damagePacket);
			}
		}

		//check if we got hit
		auto ourHealthComp = EntitySystem::GetComp<HealthComponent>(ndc.getId());
		if (ourHealthComp != nullptr && ourHealthComp->getHealth() <= 0) continue;

		const auto hurtComp = EntitySystem::GetComp<HurtboxComponent>(ndc.getId());
		if (hurtComp == nullptr) continue;

		const auto teamComp = EntitySystem::GetComp<TeamComponent>(ndc.getId());
		for (auto& otherHitComp : EntitySystem::GetPool<HitboxComponent>()) {
			if (otherHitComp.getId() == ndc.getId()) continue;

			const auto otherBaseComp = EntitySystem::GetComp<EntityBaseComponent>(otherHitComp.getId());
			if (!otherBaseComp->isActive) continue;

			const auto otherNDC = EntitySystem::GetComp<NetworkDataComponent>(otherHitComp.getId());
			if (otherNDC->owner != NetworkDataComponent::Owner::foreign) continue;

			if (teamComp->teamId != TeamId::neutral &&
                otherHitComp.getTeamId() != TeamId::neutral && 
                teamComp->teamId == otherHitComp.getTeamId()) continue;
			if (!otherHitComp.hitbox.intersects(hurtComp->hurtbox)) continue;
			if (!otherHitComp.addHitEntity(hurtComp->getId())) continue;

			damageEntity(otherHitComp.getId(), ndc.getId(), damagePacket);
		}
	}

    host->bufferAllDataByChannel(0, damagePacket);
}
