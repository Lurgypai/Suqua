#pragma once
#include <string>
#include <memory>

#include "ItemAbility.h"
// an item is a thing you can pick up and use

// item entity in world vs item in inventory

class Item {
public:
	template<typename T>
	Item(const std::string& displayName_, const std::string& tag_, T&& itemAbility);
	Item(const std::string& displayName_, const std::string& tag_);
	Item(Item&& other) = default;
	// displayed for all text purposes
	const std::string displayName;
	// used to uniquely identify this item
	const std::string tag;
	// copy the underlying ability for use while equiped
	std::unique_ptr<ItemAbility> getAbility() const;
private:
	std::unique_ptr<ItemAbility> ability;
};

template<typename T>
Item::Item(const std::string& displayName_, const std::string& tag_, T&& itemAbility) :
	displayName{displayName_},
	tag{tag_},
	ability{std::make_unique<T>(std::forward<T>(itemAbility))}
{}