#pragma once
#include "ComponentMacros.h"
#include <cstdint>

class HealthWatcherComponent {
	CompMembers(HealthWatcherComponent);

public:
    HealthWatcherComponent(EntityId id_, EntityId parentId_);

	void update();
	EntityId parentId;
private:
	std::uint32_t prevHealth;
};
