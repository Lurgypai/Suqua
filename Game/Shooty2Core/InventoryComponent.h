#pragma once
#include "ComponentMacros.h"

#include <vector>
#include <string>

#include "ItemSystem.h"
#include "InventoryItem.h"


class InventoryComponent {
	CompMembers(InventoryComponent);

public:
    InventoryComponent(EntityId id_);

	void update(Scene& scene, float delta);

	void setActionItem(int slot, const Item& item);
private:
	static constexpr int SLOT_COUNT = 2;
	std::vector<InventoryItem> actionItems;
	std::vector<InventoryItem> storageItems;
};
