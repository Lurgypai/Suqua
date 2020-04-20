#pragma once
#include "EntitySystem.h"
#include "AABB.h"

class ClimbableComponent {
public:
	ClimbableComponent(EntityId id_ = 0);

	EntityId getId() const;
	AABB collider;
private:
	EntityId id;
};