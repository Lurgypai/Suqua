#pragma once
#include "EntitySystem.h"

//handles the death state of entities

class EntityBaseComponent {
public:
	// cleaned up after this is set
	bool isDead;
	// do updates
	bool isActive;

	EntityBaseComponent(EntityId id_ = 0);

	const EntityId getId() const;
private:
	EntityId id;
};

