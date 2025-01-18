#pragma once
#include "ComponentMacros.h"

#include "AABB.h"

class HurtboxComponent {
    CompMembers(HurtboxComponent);
public:
	HurtboxComponent(EntityId id_, const Vec2f& offset_, const Vec2f& res);

	void update();

	Vec2f offset;
	AABB hurtbox;
};
