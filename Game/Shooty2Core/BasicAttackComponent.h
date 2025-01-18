#pragma once

#include "ComponentMacros.h"
#include "Vec2.h"

class BasicAttackComponent {
	CompMembers(BasicAttackComponent);

public:
    BasicAttackComponent(EntityId id_,
            int delay_,
            int duration_,
            int lag_);

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
