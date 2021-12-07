#pragma once
#include "EntitySystem.h"

class ButtonComponent {
public:
	ButtonComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update();

	bool state;
private:
	EntityId id;
};