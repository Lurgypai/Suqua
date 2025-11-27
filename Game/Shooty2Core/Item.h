#pragma once
#include <string>
#include <functional>

// an item is a thing you can pick up and use

// item entity in world vs item in inventory

class Item {
public:
	using UseCallback = std::function<void()>;

	Item(const std::string& displayName_, const std::string& tag_, const UseCallback& useCallback_ = UseCallback());
	Item(const Item& other) = default;
	Item(Item&& other) = default;
	// displayed for all text purposes
	const std::string displayName;
	// used to uniquely identify this item
	const std::string tag;
	// called when this item is "used"
	const UseCallback useCallback;
	// function to call to use an item, calls the useCallback
	void onUse() const;
private:
};