#pragma once
#include "EntitySystem.h"

//handles the death state of entities

class EntityBaseComponent {
public:
	bool isDead;

	EntityBaseComponent(EntityId id_ = 0);

	const EntityId getId() const;
private:
	EntityId id;
};

