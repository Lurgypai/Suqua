#pragma once

#include "ComponentMacros.h"
#include "Vec2.h"

class BasicAttackComponent {
	CompMembers(BasicAttackComponent);

public:
	void update();

	Vec2f getHitBoxPos() const;

	int delay;
	int duration;
	int lag;
private:
	int tick;
	bool isActive_;

	EntityId hitboxId;
};