#include "Item.h"

Item::Item(const std::string& displayName_, const std::string& tag_) :
	displayName{displayName_},
	tag{tag_},
	ability{nullptr}
{}

std::unique_ptr<ItemAbility> Item::getAbility() const {
	if (ability == nullptr) return nullptr;
	return ability->clone();
}
