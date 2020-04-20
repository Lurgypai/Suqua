#pragma once
#include "EntitySystem.h"

class EditableColliderGC {
public:
	EditableColliderGC(EntityId id_ = 0);

	void update();

	const EntityId getId() const;
private:
	EntityId id;
};