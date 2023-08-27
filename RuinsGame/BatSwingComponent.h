#pragma once
#include "ComponentMacros.h"

class BatSwingComponent {
	CompMembers(BatSwingComponent);

public:
	void update();
	int delay;
	int duration;
	int lag;
private:
	int tick;
	bool isActive_;

	EntityId hitboxId;
};