#pragma once
#include "ComponentMacros.h"
#include <cstdint>

class HealthWatcherComponent {
	CompMembers(HealthWatcherComponent);

public:
	void update();
	EntityId parentId;
private:
	std::uint32_t prevHealth;
};
