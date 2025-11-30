#pragma once
#include "ComponentMacros.h"

#include <vector>
#include <string>
#include <unordered_map>

#include "ItemSystem.h"
#include "InventoryItem.h"


class InventoryComponent {
	CompMembers(InventoryComponent);

public:
	// number of hands
	static constexpr int SLOT_COUNT = 2;

    InventoryComponent(EntityId id_, Vec2f bodyOffset_, float handOffset_);

	void update(Scene& scene, float delta);

	void setActionItem(int slot, const Item& item);
	void setActionItem(int slot, const Item& item, EntityId targetEntity);

	Vec2f getBodyPos() const;
	Vec2f getHandPos(int index) const;
	float getHandAngle(int index) const;
	const std::string& getHandTag(int index) const;
	bool handIsActive(int index) const;

	// offset to center of arms from entity pos
	Vec2f bodyOffset;
	// offset from body pos to hands
	float handOffset;

	// what controller buttons to use for each hand
	std::vector<ControllerBits> handFlags;
private:
	std::vector<Vec2f> handTargetPos;
	bool handsAligned;
	float handMoveRate;
	// items in hands
	std::vector<InventoryItem> actionItems;
	// other items
	std::vector<InventoryItem> storageItems;
};
