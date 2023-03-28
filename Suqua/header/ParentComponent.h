#pragma once

#include "EntitySystem.h"
#include "Vec2.h"

// links to a parent entity in space, moves to it, updated by "offset"
class ParentComponent {
public:
	ParentComponent(EntityId id_ = 0);
	EntityId getId() const;

	EntityId parentId;
	Vec2f offset;

	void update();
private:
	EntityId id;
};