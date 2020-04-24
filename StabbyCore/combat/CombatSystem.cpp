#include "CombatSystem.h"
#include "PhysicsComponent.h"
#include "DirectionComponent.h"

CombatSystem::CombatSystem() {}

void CombatSystem::runAttackCheck(double timeDelta) {
	if (EntitySystem::Contains<CombatComponent>()) {
		for (CombatComponent & attacker : EntitySystem::GetPool<CombatComponent>()) {

			updateState(attacker, timeDelta);

			for (CombatComponent& defender : EntitySystem::GetPool<CombatComponent>()) {
				attackCheck(attacker, defender);
			}
		}
	}
}

void CombatSystem::runAttackCheck(double timeDelta, EntityId id) {
	if (EntitySystem::Contains<CombatComponent>()) {
		for (CombatComponent& attacker : EntitySystem::GetPool<CombatComponent>()) {

			//if we're the attacker, loop through all the defenders and attack them
			if (attacker.getId() == id) {
				updateState(attacker, timeDelta);
				for (CombatComponent& defender : EntitySystem::GetPool<CombatComponent>()) {
					attackCheck(attacker, defender);
				}
			}
			else {
				//if we aren't the attacker, only update our defense (don't update everyone elses defence)
				CombatComponent& defender = *EntitySystem::GetComp<CombatComponent>(id);
				attackCheck(attacker, defender);
			}
		}
	}
}

void CombatSystem::updateState(CombatComponent& attacker, double timeDelta) {
	attacker.updateHurtboxes();
	attacker.updateStun();
	attacker.updateStamina();
	attacker.updateFreezeFrame();

	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(attacker.getId());
	DirectionComponent* direction = EntitySystem::GetComp<DirectionComponent>(attacker.getId());

	if (physics != nullptr) {
		if (!attacker.isFrozen())
			physics->frozen = false;
		else
			physics->frozen = true;

		if (!physics->frozen)
			attacker.attack.update(timeDelta, physics->getPos(), direction->dir);
	}
}

void CombatSystem::attackCheck(CombatComponent& attacker, CombatComponent& defender) {
	EntityId attackerId = attacker.getId();
	EntityId defenderId = defender.getId();
	if (attackerId != defenderId) {
		if(defender.isAlive()) {
			if (attacker.teamId != 0 && defender.teamId != 0 && attacker.teamId != defender.teamId) {
				bool attackChanged = attacker.attack.pollAttackChange();
				if (attackChanged) {
					attacker.clearHitEntities();
					uint32_t staminaCost = attacker.getStaminaCost();
					if (staminaCost < attacker.stamina)
						attacker.useStamina(staminaCost);
					else
						attacker.useStamina(attacker.stamina);
				}
				if (attackChanged || !attacker.hasHitEntity(defenderId)) {
					auto* activeHitbox = attacker.getActiveHitbox();
					if (activeHitbox != nullptr) {
						const AABB& hit = activeHitbox->hit;
						if (hit.intersects(defender.getBoundingBox())) {
							auto hurtboxes = defender.hurtboxes;
							for (auto& hurtbox : hurtboxes) {
								if (hurtbox.box.intersects(hit)) {
									defender.damage(attacker.rollDamage());
									defender.stun(attacker.getStun());
									defender.lastAttacker = attackerId;
									attacker.onAttackLand();

									attacker.addHitEntity(defenderId);

									attacker.freeze();
									defender.freeze();
								}
							}
						}
					}
				}
			}
		}
	}
}
