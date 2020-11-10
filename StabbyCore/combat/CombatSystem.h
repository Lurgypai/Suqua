#pragma once
#include "EntitySystem.h"
#include "../player/PlayerLC.h"
#include "PhysicsComponent.h"
#include "Pool.h"
#include "CombatComponent.h"
#include <unordered_map>

//defined in CombatComponent.h
class CombatComponent;

class CombatSystem {
public:
	CombatSystem();

	void runAttackCheck(double timeDelta);
	void runAttackCheck(double timeDelta, EntityId id);
	void updateState(CombatComponent& combat, double timeDelta);
private:
	void attackCheck(CombatComponent& attacker, CombatComponent& defender);
	bool isInRange(EntityId attackerId, EntityId defenderId);
	const unsigned int MIN_ATTACK_DISTANCE = 120;
	int prevAttackId;

	std::unordered_map<EntityId, std::unordered_map<EntityId, bool>> rangeChecks;
};
