#pragma once
#include "ComponentMacros.h"

class TopDownMoverComponent {
    CompMembers(TopDownMoverComponent);
public:
	TopDownMoverComponent(EntityId id_, float moveSpeed_);

	void update();
	float getMoveSpeed();
	void setMoveSpeed(float moveSpeed);
private:
	float* moveSpeed;
};
