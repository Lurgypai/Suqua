#include "ItemSystem.h"

void ItemSystem::registerItem(Item&& item) {
	items.emplace(item.getTag(), std::forward<Item>(item));
}

const Item& ItemSystem::getItem(const std::string& tag) const {
	return items.at(tag);
}

bool ItemSystem::hasItem(const std::string& tag) const {
	return items.contains(tag);
}
