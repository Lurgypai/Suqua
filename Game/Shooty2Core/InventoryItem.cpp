#include "InventoryItem.h"

InventoryItem::InventoryItem(const std::string& tag_, int count_, std::unique_ptr<ItemAbility>&& ability_) :
	tag{tag_},
	count{count_},
	ability{std::move(ability_)}
{}

InventoryItem::InventoryItem(const InventoryItem& other) :
	tag{ other.tag },
	count{ other.count },
	ability{ other.ability->clone() }
{}

InventoryItem& InventoryItem::operator=(const InventoryItem& other) {
	tag = other.tag;
	count = other.count;
	ability = other.ability->clone();

	return *this;
}
