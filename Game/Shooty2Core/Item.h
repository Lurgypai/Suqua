#pragma once
#include <string>
#include <memory>

#include "ItemAbility.h"
// an item is a thing you can pick up and use

// this is the template for the raw idea of an item
// the in WorldItem entity and InventoryItem object reference this as their underlying item

class Item {
public:
    Item() = default;
	template<typename T>
	Item(const std::string& tag_, T&& itemAbility);
	Item(const std::string& tag_);
	Item(Item&& other) = default;
    Item(const Item& other);
    Item& operator=(Item&& other) = default;
    Item& operator=(const Item& other);

    const std::string& getTag() const;
	const ItemAbility* getAbility() const;
    ItemAbility* getAbility();
private:
	std::string tag;
	std::unique_ptr<ItemAbility> ability;
};

template<typename T>
Item::Item(const std::string& tag_, T&& itemAbility) :
	tag{tag_},
	ability{std::make_unique<T>(std::forward<T>(itemAbility))}
{}
