#pragma once

#include "ComponentMacros.h"
#include "Vec2.h"

// links to a parent entity in space, moves to it, updated by "offset"
class ParentComponent {
    CompMembers(ParentComponent);
public:
	enum class OffsetMode {
		none, // the effected offset isn't used
		cardinal, // flip on up down left right depending on dir
		cardinal_left_right, 
		cardinal_up_down,
		radial // rotate around depending on dir
	};

	ParentComponent(EntityId id_,
            OffsetMode offsetMode_,
            EntityId parentId_,
            const Vec2f& baseOffset_,
            const Vec2f& effectedOffset_ );

	OffsetMode offsetMode;
	EntityId parentId;
    // base amount to offset from parent
	Vec2f baseOffset;
	// offset effected by the offset mode
	Vec2f effectedOffset;

	void update();
private:
};
