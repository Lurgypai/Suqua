#pragma once
#include "EntitySystem.h"

struct EntityAddAIGunnerArgs {
    float followRadius;
	float avoidRadius;
};
    
extern void EntityAddAIGunner(EntityId id, const EntityAddAIGunnerArgs& args);
