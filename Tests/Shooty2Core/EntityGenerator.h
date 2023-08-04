#pragma once
#include "EntitySystem.h"
#include "TeamComponent.h"

namespace EntityGenerator {
	void MakePlayer(EntityId playerId, EntityId gunId, TeamId team);
	void MakeEnemy(EntityId enemy, TeamId team);
}