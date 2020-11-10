#include "CombatSystem.h"
#include "PhysicsComponent.h"
#include "DirectionComponent.h"

CombatSystem::CombatSystem() {}

void CombatSystem::runAttackCheck(double timeDelta) {
	std::list<EntityId> stunnedDefenders{};

	if (EntitySystem::Contains<CombatComponent>()) {
		for (CombatComponent & attacker : EntitySystem::GetPool<CombatComponent>()) {

			updateState(attacker, timeDelta);

			for (CombatComponent& defender : EntitySystem::GetPool<CombatComponent>()) {
				attackCheck(attacker, defender);
				if (attacker.getId() != defender.getId() && defender.isStunned())
					stunnedDefenders.push_back(defender.getId());
			}
		}
		for (auto& defenderId : stunnedDefenders) {
			CombatComponent* defender = EntitySystem::GetComp<CombatComponent>(defenderId);
			defender->attack->cancelAttack();
		}
	}

	rangeChecks.clear();
}

void CombatSystem::runAttackCheck(double timeDelta, EntityId id) {
	if (EntitySystem::Contains<CombatComponent>()) {
		std::list<EntityId> stunnedDefenders{};
		bool weStunned = false;

		for (CombatComponent& attacker : EntitySystem::GetPool<CombatComponent>()) {

			//if we're the attacker, loop through all the defenders and attack them
			if (attacker.getId() == id) {
				updateState(attacker, timeDelta);
				for (CombatComponent& defender : EntitySystem::GetPool<CombatComponent>()) {
					attackCheck(attacker, defender);
					if (attacker.getId() != defender.getId() && defender.isStunned())
						stunnedDefenders.push_back(defender.getId());
				}
			}
			else {
				//if we aren't the attacker, only update our defense (don't update everyone elses defence)
				CombatComponent& defender = *EntitySystem::GetComp<CombatComponent>(id);
				attackCheck(attacker, defender);
				//only add us once
				if (!weStunned && defender.isStunned()) {
					weStunned = true;
					stunnedDefenders.push_back(defender.getId());
				}
			}
		}

		for (auto& defenderId : stunnedDefenders) {
			CombatComponent* defender = EntitySystem::GetComp<CombatComponent>(defenderId);
			defender->attack->cancelAttack();
		}
	}

	rangeChecks.clear();
}

void CombatSystem::updateState(CombatComponent& attacker, double timeDelta) {
	attacker.updateHurtboxes();
	attacker.updateFreezeFrame();

	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(attacker.getId());
	DirectionComponent* direction = EntitySystem::GetComp<DirectionComponent>(attacker.getId());

	if (physics != nullptr) {
		if (!attacker.isFrozen())
			physics->frozen = false;
		else
			physics->frozen = true;

		if (!attacker.isFrozen()) {
			attacker.updateStun();
			attacker.updateStamina();
			attacker.attack->update(timeDelta, physics->getPos(), direction->dir);
		}
	}
}

void CombatSystem::attackCheck(CombatComponent& attacker, CombatComponent& defender) {
	EntityId attackerId = attacker.getId();
	EntityId defenderId = defender.getId();

	if (attackerId != defenderId) {
		if (defender.isAlive()) {
			if (attacker.teamId != 0 && defender.teamId != 0 && attacker.teamId != defender.teamId) {
				if (isInRange(attackerId, defenderId)) {
					bool attackChanged = attacker.attack->pollAttackChange();
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
										defender.stun(attacker.getStun() + attacker.getAttack().getCurrRemainingFrames());
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
}

bool CombatSystem::isInRange(EntityId attackerId, EntityId defenderId) {
	EntityId greaterId = std::max(attackerId, defenderId);
	EntityId lesserId = std::min(attackerId, defenderId);
	if (rangeChecks.find(greaterId) != rangeChecks.end()) {
		if (rangeChecks[greaterId].find(lesserId) != rangeChecks[greaterId].end()) {
			return rangeChecks[greaterId][lesserId];
		}
	}

	PhysicsComponent* greaterPhysics = EntitySystem::GetComp<PhysicsComponent>(greaterId);
	PhysicsComponent* lesserPhysics = EntitySystem::GetComp<PhysicsComponent>(lesserId);

	bool inRange = greaterPhysics->getPos().distance(lesserPhysics->getPos()) < MIN_ATTACK_DISTANCE;
	rangeChecks[greaterId][lesserId] = inRange;
	return inRange;
}
