#pragma once

#include <vector>
#include <string>

#include "EntitySystem.h"

struct EntityAddInventoryArgs {
	std::vector<std::string> itemTags;
	std::vector<int> itemCounts;
};

extern void EntityAddInventory(EntityId id, const EntityAddInventoryArgs& args);

