#pragma once
#include "EntitySystem.h"

class PhysicsSystem {
public:
	PhysicsSystem();
	
	void runPhysics(double timeDelta);
    void runPhysicsOnOwned(double timeDelta);

	void runPhysics(double timeDelta, EntityId entity);

};
