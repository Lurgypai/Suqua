#pragma once
#include "EntitySystem.h"

class NameTagGC {
public:
	NameTagGC(EntityId id_ = 0);
	EntityId getId() const;
private:
	EntityId id;
};