#pragma once
#include "EntitySystem.h"

class GunGFXComponent {
public:
	GunGFXComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update();
private:
	EntityId id;
};