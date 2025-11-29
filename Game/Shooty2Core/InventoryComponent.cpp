#include "InventoryComponent.h"
#include "ControllerComponent.h"
#include "PhysicsComponent.h"
#include "PositionComponent.h"

#include <print>

InventoryComponent::InventoryComponent(EntityId id_, Vec2f bodyOffset_, float handOffset_) :
	id{ id_ },
	handFlags{ ControllerBits::BUTTON_11, ControllerBits::BUTTON_12 },
	actionItems{},
	storageItems{},
	bodyOffset{bodyOffset_},
	handOffset{handOffset_},
	handTargetPos{},
	handsAligned{false},
	handMoveRate{0.2f}
{
	actionItems.resize(SLOT_COUNT);
	handTargetPos.resize(SLOT_COUNT);
}

void InventoryComponent::update(Scene& scene, float delta) {
	// update items
	for (auto& item : actionItems) {
		if (item.ability != nullptr) item.ability->update(delta);
	}

	// use items
	auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);
	if (contComp == nullptr) return;
	const auto& controller = contComp->getController();

	// toggle between aligned and spread
	if (controller.toggled(ControllerBits::BUTTON_8)) {
		if (controller[ControllerBits::BUTTON_8]) handsAligned = !handsAligned;
	}

	// get base body position
	Vec2f basePos = getBodyPos();
	// calculate offset to hands
	Vec2f baseArmOffset = Vec2f{ handOffset, 0.f };
	baseArmOffset.angle(controller.stick2.angle());

	for (int i = 0; i != SLOT_COUNT; ++i) {
		auto& leftItem = actionItems[i];
		if (!leftItem.tag.empty()) {
			// switch between left and right
			int sign = (i % 2 == 0 ? 1 : -1);
			// calculate hand target position
			if (handsAligned) {
				handTargetPos[i] = basePos + baseArmOffset * sign;
			}
			else {
				float angleOffset = 3.1415926535898 / 4.f;
				Vec2f armOffset = baseArmOffset;
				armOffset.angle(armOffset.angle() + angleOffset * -sign);
				handTargetPos[i] = basePos + armOffset;
			}
			// move hand and set angle
			Vec2f delta = handTargetPos[i] - leftItem.heldPos;
			leftItem.heldPos += delta * handMoveRate;
			leftItem.angle = baseArmOffset.angle();
			// activate abilities
			if (controller[handFlags[i]] && leftItem.ability != nullptr) {
				leftItem.ability->doAbility(scene, id, controller, leftItem);
			}
		}
	}
}

void InventoryComponent::setActionItem(int slot, const Item& item) {
	setActionItem(slot, item, id);
}

void InventoryComponent::setActionItem(int slot, const Item& item, EntityId targetEntity) {
	// maybe crash?
	if (slot < 0 && slot > SLOT_COUNT - 1) return;

	actionItems[slot] = InventoryItem{ item.tag, 1, item.getAbility()};
	if (actionItems[slot].ability == nullptr) return;
	actionItems[slot].ability->targetEntity = targetEntity;
}

Vec2f InventoryComponent::getBodyPos() const {
	auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	if(physicsComp) return physicsComp->position() + bodyOffset;
	auto* positionComp = EntitySystem::GetComp<PositionComponent>(id);
	return positionComp->pos + bodyOffset;
}

Vec2f InventoryComponent::getHandPos(int hand) const {
	if (hand < 0 || hand > SLOT_COUNT - 1) return Vec2f{};
	return actionItems[hand].heldPos;
}

float InventoryComponent::getHandAngle(int hand) const {
	if (hand < 0 || hand > SLOT_COUNT - 1) return 0.f;
	return actionItems[hand].angle;
}

const std::string& InventoryComponent::getHandTag(int hand) const {
	// more elegant crashes?
	return actionItems.at(hand).tag;
}

bool InventoryComponent::handIsActive(int hand) const {
	if (hand < 0 || hand > SLOT_COUNT - 1) return false;
	return !actionItems[hand].tag.empty();
}
