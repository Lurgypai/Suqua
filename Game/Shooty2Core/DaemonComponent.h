#pragma once
#include "ComponentMacros.h"
#include "Vec2.h"

class DaemonComponent {
	CompMembers(DaemonComponent);

public:
    DaemonComponent(EntityId id_, float followRate_, Vec2f followOffset_, float maxDistance_ = 30.f);

	float followRate;
	Vec2f followOffset;
    float maxDistance;

	// entity to follow
	EntityId hostEntity;
	void update(double timeDelta);
private:
	bool isHolding;
	Vec2f targetPos;
	int side;
};
