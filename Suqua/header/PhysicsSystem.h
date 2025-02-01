#pragma once
#include "EntitySystem.h"
#include "PhysicsComponent.h"

class PhysicsSystem {
public:
	PhysicsSystem();
	
	void runPhysics(double timeDelta);
    void runPhysicsOnOwned(double timeDelta);

	void runPhysics(double timeDelta, PhysicsComponent& physicsComp);

private:
    // this is a bit of an ugly solution, but it should improve performance enough for now.
    void getActive();
    void getActiveOwned();
    std::vector<PhysicsComponent*> active;
    std::vector<PhysicsComponent*> collidesWith;
};
