#include "ItemSystem.h"

void ItemSystem::registerItem(const Item& item) {
	items.emplace(item.tag, item);
}

const Item& ItemSystem::getItem(const std::string& tag) {
	return items.at(tag);
}

void ItemSystem::useItem(const std::string& tag) const {
	items.at(tag).onUse();
}
