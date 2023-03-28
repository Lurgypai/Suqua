#pragma once

#include "EntitySystem.h"

class AimToMouseComponent {
public:
	AimToMouseComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update();
private:
	EntityId id;
};