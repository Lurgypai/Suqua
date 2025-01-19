#pragma once
#include "ComponentMacros.h"
#include "Vec2.h"

class PositionComponent {
    CompMembers(PositionComponent);
public:
    PositionComponent(EntityId id_, const Vec2f& pos = Vec2f{});

	Vec2f getPos() const;
	void setPos(const Vec2f& newPos);
private:
	float* xPos;
	float* yPos;
};
