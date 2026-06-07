#pragma once

#include "Item.h"
#include "Vec2.h"

class HandItem {
public:
    HandItem() = default;
	HandItem(const Item& item_, int count_, EntityId targetEntity_);
	HandItem(const HandItem& other) = default;
	HandItem(HandItem&& other) = default;
    HandItem& operator=(const HandItem& other) = default;
    HandItem& operator=(HandItem&& other) = default;

    Item item;
    EntityId targetEntity;
	Vec2f heldPos;
	float angle;
};
