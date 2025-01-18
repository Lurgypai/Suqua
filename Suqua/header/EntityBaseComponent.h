#pragma once
#include "ComponentMacros.h"

//handles the death state of entities

class EntityBaseComponent {
    CompMembers(EntityBaseComponent);
public:
	// cleaned up after this is set
	bool isDead;
	// do updates
	bool isActive;

	EntityBaseComponent(EntityId id_);
private:
};

