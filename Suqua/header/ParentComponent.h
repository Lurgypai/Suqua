#pragma once

#include "EntitySystem.h"
#include "Vec2.h"

// links to a parent entity in space, moves to it, updated by "offset"
class ParentComponent {
public:
	ParentComponent(EntityId id_ = 0);
	EntityId getId() const;

	EntityId parentId;
	Vec2f baseOffset;

	enum class OffsetMode {
		none, // the effected offset isn't used
		cardinal, // flip on up down left right depending on dir
		cardinal_left_right, 
		cardinal_up_down,
		radial // rotate around depending on dir
	};

	OffsetMode offsetMode;
	// offset effected by the offset mode
	Vec2f effectedOffset;

	void update();
private:
	EntityId id;
};