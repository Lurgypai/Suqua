#include "InventoryItem.h"

InventoryItem::InventoryItem(const std::string& tag_, int count_, std::unique_ptr<ItemAbility>&& ability_) :
	tag{tag_},
	count{count_},
	ability{std::move(ability_)},
	heldPos{},
	angle{}
{}

InventoryItem::InventoryItem(const InventoryItem& other) :
	tag{ other.tag },
	count{ other.count },
	ability{ other.ability->clone() },
	heldPos{other.heldPos},
	angle{other.angle}
{}

InventoryItem& InventoryItem::operator=(const InventoryItem& other) {
	tag = other.tag;
	count = other.count;
	ability = other.ability->clone();
	heldPos = other.heldPos;
	angle = other.angle;

	return *this;
}
