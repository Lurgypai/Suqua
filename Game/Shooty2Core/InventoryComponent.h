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

	void update(const ItemSystem& itemSystem);

	void setActionItem(int slot, const std::string& tag);
private:
	std::vector<InventoryItem> actionItems;
	std::vector<InventoryItem> storageItems;
};
