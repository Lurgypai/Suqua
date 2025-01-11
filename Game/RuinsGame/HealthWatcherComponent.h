#pragma once
#include "ComponentMacros.h"

class HealthWatcherComponent {
	CompMembers(HealthWatcherComponent);

public:
	void update();
	EntityId parentId;
private:
	std::uint32_t prevHealth;
};
