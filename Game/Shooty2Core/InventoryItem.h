#pragma once

#include "Item.h"
#include "Vec2.h"

class InventoryItem {
public:
    InventoryItem() = default;
	InventoryItem(const Item& item_, int count_);
	InventoryItem(const InventoryItem& other) = default;
	InventoryItem(InventoryItem&& other) = default;
    InventoryItem& operator=(const InventoryItem& other) = default;
    InventoryItem& operator=(InventoryItem&& other) = default;

    Item item;
	int count;
	Vec2f heldPos;
	float angle;
};
