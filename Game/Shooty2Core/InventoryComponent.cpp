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

inline static Vec2f getTargetHandPos(int hand, bool handsAligned, const Vec2f& basePos, float armLength, float angle) {
	Vec2f targetPos;
	// switch between left and right
	int sign = (hand % 2 == 0 ? 1 : -1);
	// calculate hand target position
	Vec2f baseArmOffset{ armLength, 0.f };
	baseArmOffset.angle(angle);
	if (handsAligned) {
		targetPos = basePos + baseArmOffset * sign;
	}
	else {
		float angleOffset = 3.1415926535898 / 4.f;
		Vec2f armOffset = baseArmOffset;
		armOffset.angle(armOffset.angle() + angleOffset * -sign);
		targetPos = basePos + armOffset;
	}
	return targetPos;
}

void InventoryComponent::update(Scene& scene, float delta) {
	// update items
	for (int hand = 0; hand != actionItems.size(); ++hand) {
		auto& item = actionItems[hand];
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

	for (int i = 0; i != SLOT_COUNT; ++i) {
		auto& leftItem = actionItems[i];
		if (!leftItem.tag.empty()) {
			handTargetPos[i] = getTargetHandPos(i, handsAligned, basePos, handOffset, controller.stick2.angle());
			// move hand and set angle
			Vec2f delta = handTargetPos[i] - leftItem.heldPos;
			leftItem.heldPos += delta * handMoveRate;
			leftItem.angle = controller.stick2.angle();
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
	auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);
	actionItems[slot].heldPos = getTargetHandPos(slot, handsAligned, getBodyPos(), handOffset,
		contComp->getController().stick2.angle());
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
