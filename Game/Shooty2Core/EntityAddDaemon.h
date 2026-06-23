#pragma once
#include "EntitySystem.h"
#include "Vec2.h"

struct EntityAddDaemonArgs {
	float followRate;
	Vec2f followOffset;
};

extern void EntityAddDaemon(EntityId id, const EntityAddDaemonArgs& args);