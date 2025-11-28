#pragma once
#include "ComponentMacros.h"

#include <vector>
#include <string>

#include "ItemSystem.h"
#include "InventoryItem.h"


class InventoryComponent {
	CompMembers(InventoryComponent);

public:
    InventoryComponent(EntityId id_, Vec2f bodyOffset_, float handOffset_);

	void update(Scene& scene, float delta);

	void setActionItem(int slot, const Item& item);

	Vec2f getBodyPos() const;
	Vec2f getHandPos(int index);
	float getHandAngle(int index);

	// offset to center of arms from entity pos
	Vec2f bodyOffset;
	// offset from body pos to hands
	float handOffset;

	ControllerBits lHandFlag;
	ControllerBits rHandFlag;
private:
	// number of hands
	static constexpr int SLOT_COUNT = 2;
	// items in hands
	std::vector<InventoryItem> actionItems;
	// other items
	std::vector<InventoryItem> storageItems;
	// controller buttons for left and right hands
};
