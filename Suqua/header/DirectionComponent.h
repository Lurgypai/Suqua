#pragma once
#include "EntitySystem.h"

class DirectionComponent {
public:
	DirectionComponent(EntityId id_ = 0);
	EntityId getId() const;

	int dir;
private:
	EntityId id;
};