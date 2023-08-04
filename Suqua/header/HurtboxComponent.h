#pragma once
#include "EntitySystem.h"
#include "AABB.h"

class HurtboxComponent {
public:
	HurtboxComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update();

	Vec2f offset;
	AABB hurtbox;
private:
	EntityId id;
};