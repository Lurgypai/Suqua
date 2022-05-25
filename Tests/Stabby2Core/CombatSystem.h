#pragma once
#include <unordered_map>
#include "EntitySystem.h"
#include "Vec2.h"

class CombatSystem {
public:
	void update();
private:
	std::unordered_map<EntityId, Vec2f> launchData;
};