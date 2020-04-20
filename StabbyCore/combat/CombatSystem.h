#pragma once
#include "EntitySystem.h"
#include "../player/PlayerLC.h"
#include "PhysicsComponent.h"
#include "Pool.h"
#include "CombatComponent.h"

//defined in CombatComponent.h
class CombatComponent;

class CombatSystem {
public:
	CombatSystem();

	void runAttackCheck(double timeDelta);
	void runAttackCheck(double timeDelta, EntityId id);
private:

	void attackCheck(CombatComponent& attacker, CombatComponent& defender, bool attackChange);
	int prevAttackId;
};
