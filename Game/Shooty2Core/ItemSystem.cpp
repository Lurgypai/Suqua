#include "ItemSystem.h"

void ItemSystem::registerItem(Item&& item) {
	items.emplace(item.tag, std::forward<Item>(item));
}

const Item& ItemSystem::getItem(const std::string& tag) {
	return items.at(tag);
}

bool ItemSystem::hasItem(const std::string& tag) {
	return items.contains(tag);
}
