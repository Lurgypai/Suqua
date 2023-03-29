#pragma once
#include "EntitySystem.h"
#include "Vec2.h"

class PositionComponent {
public:
	PositionComponent(EntityId id_ = 0);
	EntityId getId() const;

	Vec2f getPos() const;
	void setPos(const Vec2f& newPos);
private:
	EntityId id;

	float* xPos;
	float* yPos;
};