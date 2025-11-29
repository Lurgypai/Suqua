#pragma once
#include "ComponentMacros.h"
#include "Vec2.h"

class DaemonComponent {
	CompMembers(DaemonComponent);

public:
    DaemonComponent(EntityId id_, float followRate_, Vec2f followOffset_);

	float followRate;
	Vec2f followOffset;

	// entity to follow
	EntityId hostEntity;
	void update();
private:
	bool isHolding;
	Vec2f targetPos;
};
