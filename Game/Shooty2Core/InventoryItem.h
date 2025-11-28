#pragma once
#include <string>
#include <memory>

#include "ItemAbility.h"

class ItemAbility;

class InventoryItem {
public:
	InventoryItem() = default;
	InventoryItem(const std::string& tag_, int count_, std::unique_ptr<ItemAbility>&& ability);
	InventoryItem(const InventoryItem& other);
	InventoryItem(InventoryItem&& other) = default;
	InventoryItem& operator=(const InventoryItem& other);
	InventoryItem& operator=(InventoryItem&& other) = default;

	std::string tag;
	int count;
	std::unique_ptr<ItemAbility> ability;
};
