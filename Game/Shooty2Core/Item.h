#pragma once
#include <string>
#include <memory>

#include "ItemAbility.h"
// an item is a thing you can pick up and use

// item entity in world vs item in inventory

class Item {
public:
	template<typename T>
	Item(const std::string& displayName_, const std::string& tag_, bool doesTargetHost, T&& itemAbility);
	Item(const std::string& displayName_, const std::string& tag_, bool doesTargetHost_);
	Item(Item&& other) = default;
	// displayed for all text purposes
	const std::string displayName;
	// used to uniquely identify this item
	const std::string tag;
	// item should target the "host", mostly for daemon
	bool doesTargetHost;

	// copy the underlying ability for use while equiped
	std::unique_ptr<ItemAbility> getAbility() const;
private:
	std::unique_ptr<ItemAbility> ability;
};

template<typename T>
Item::Item(const std::string& displayName_, const std::string& tag_, bool doesTargetHost_, T&& itemAbility) :
	displayName{displayName_},
	tag{tag_},
	doesTargetHost{doesTargetHost_},
	ability{std::make_unique<T>(std::forward<T>(itemAbility))}
{}