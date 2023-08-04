#pragma once
#include "EntitySystem.h"

class TopDownMoverComponent {
public:
	TopDownMoverComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update();
	float getMoveSpeed();
	void setMoveSpeed(float moveSpeed);
private:
	float* moveSpeed;
	EntityId id;
};