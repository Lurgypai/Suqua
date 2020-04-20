
#include "CombatSystem.h"
#include "PhysicsComponent.h"
#include "DirectionComponent.h"

CombatSystem::CombatSystem() {}

void CombatSystem::runAttackCheck(double timeDelta) {
	if (EntitySystem::Contains<CombatComponent>()) {
		for (CombatComponent & attacker : EntitySystem::GetPool<CombatComponent>()) {

			PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(attacker.getId());
			DirectionComponent * direction = EntitySystem::GetComp<DirectionComponent>(attacker.getId());

			if (physics != nullptr && direction != nullptr) {
				attacker.updateHurtboxes();
				attacker.updateStun();
				attacker.updateStamina();

				if (physics != nullptr && !physics->frozen)
					attacker.attack.update(timeDelta, physics->getPos(), direction->dir);


				bool attackChanged = attacker.attack.pollAttackChange();
				if (attackChanged) {
					attacker.clearHitEntities();
					uint32_t staminaCost = attacker.getStaminaCost();
					if (staminaCost < attacker.stamina)
						attacker.useStamina(staminaCost);
					else
						attacker.useStamina(attacker.stamina);
				}

				for (CombatComponent& defender : EntitySystem::GetPool<CombatComponent>()) {
					attackCheck(attacker, defender, attackChanged);
				}
			}
		}
	}
}

void CombatSystem::runAttackCheck(double timeDelta, EntityId id) {
	if (EntitySystem::Contains<CombatComponent>()) {
		for (CombatComponent& attacker : EntitySystem::GetPool<CombatComponent>()) {

			PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(attacker.getId());
			DirectionComponent* direction = EntitySystem::GetComp<DirectionComponent>(attacker.getId());

			//if we're the attacker, loop through all the defenders and attack them
			if (attacker.getId() == id) {
				attacker.updateHurtboxes();
				attacker.updateStun();
				attacker.updateStamina();
			
				if (physics != nullptr && !physics->frozen)
					attacker.attack.update(timeDelta, physics->getPos(), direction->dir);
				
				bool attackChanged = attacker.attack.pollAttackChange();
				if (attackChanged) {
					attacker.clearHitEntities();
					uint32_t staminaCost = attacker.getStaminaCost();
					if (staminaCost < attacker.stamina)
						attacker.useStamina(staminaCost);
					else
						attacker.useStamina(attacker.stamina);
				}

				for (CombatComponent& defender : EntitySystem::GetPool<CombatComponent>()) {
					attackCheck(attacker, defender, attackChanged);
				}
			}
			else {
				//if we aren't the attacker, only update our defense
				CombatComponent& defender = *EntitySystem::GetComp<CombatComponent>(id);

				bool attackChanged = defender.attack.pollAttackChange();
				if (attackChanged) {
					defender.clearHitEntities();
					uint32_t staminaCost = defender.getStaminaCost();
					if (staminaCost < defender.stamina)
						defender.useStamina(staminaCost);
					else
						defender.useStamina(defender.stamina);
				}

				attackCheck(attacker, defender, attackChanged);
			}
		}
	}
}

void CombatSystem::attackCheck(CombatComponent& attacker, CombatComponent& defender, bool attackChanged) {
	EntityId attackerId = attacker.getId();
	EntityId defenderId = defender.getId();
	if (attackerId != defenderId) {
		if (attacker.teamId != 0 && defender.teamId != 0 && attacker.teamId != defender.teamId) {

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
							}
						}
					}
				}
			}
		}
	}
}
