#pragma once
#include "EntitySystem.h"

class PhysicsSystem {
public:
	PhysicsSystem();
	
	void runPhysics(double timeDelta);
    void runPhysicsOnOwned(double timeDelta);

	void runPhysics(double timeDelta, EntityId entity);

private:
    // this is a bit of an ugly solution, but it should improve performance enough for now.
    void getActive();
    std::vector<EntityId> active;
    std::vector<EntityId> collidesWith;
};
