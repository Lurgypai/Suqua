#pragma once
#include "ComponentMacros.h"

#include <vector>
#include <string>

#include "Item.h"
#include "HandItem.h"


class HandComponent {
	CompMembers(HandComponent);

public:
	// number of hands
	static constexpr int SLOT_COUNT = 2;

	HandComponent(EntityId id_, Vec2f bodyOffset_, float handOffset_,
		const std::vector<ControllerBits>& handFlags_ = { ControllerBits::BUTTON_5, ControllerBits::BUTTON_6 });

	void update(Scene& scene, float delta);

	void setItem(int slot, const Item& item);
	void setItem(int slot, const Item& item, EntityId targetEntity);
    void clearItem(int slot);

	Vec2f getBodyPos() const;
	Vec2f getHandPos(int index) const;
	float getHandAngle(int index) const;
	const std::string& getHandTag(int index) const;
	bool handIsActive(int index) const;

	// offset to center of arms from entity pos
	Vec2f bodyOffset;
	// offset from body pos to hands
	float handOffset;

private:
	// what controller buttons to use for each hand
	std::vector<ControllerBits> handFlags;

	std::vector<Vec2f> handTargetPos;
	bool handsAligned;
	float handMoveRate;
	// items in hands
	std::vector<HandItem> actionItems;
	// other items
	std::vector<HandItem> storageItems;
};
