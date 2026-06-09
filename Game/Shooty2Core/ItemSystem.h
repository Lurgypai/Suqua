#pragma once
#include <unordered_map>

#include "Item.h"

// this class is in charge of storing all of the item types
// items in this master list are referenced when interacted with

class ItemSystem {
public:
    void loadItems(const std::string& filename);
	// add an item to the item registry 
	
	// retrieve item from registry
	const Item& getItem(const std::string& tag) const;
	bool hasItem(const std::string& tag) const;

    const std::unordered_map<std::string, Item>& getItems() const;
private:
	std::unordered_map<std::string, Item> items;
};
