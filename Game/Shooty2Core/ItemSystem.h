#pragma once
#include <unordered_map>

#include "Item.h"

// this class is in charge of storing all of the item types
// items in this master list are referenced when interacted with

class ItemSystem {
public:
	// add an item to the item registry 
	void registerItem(Item&& item);
	
	// retrieve item from registry
	const Item& getItem(const std::string& tag);
private:
	std::unordered_map<std::string, Item> items;
};