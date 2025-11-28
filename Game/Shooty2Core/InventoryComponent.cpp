#include "InventoryComponent.h"
#include "ControllerComponent.h"
#include "PhysicsComponent.h"

InventoryComponent::InventoryComponent(EntityId id_, Vec2f bodyOffset_, float handOffset_) :
	id{ id_ },
	lHandFlag{ControllerBits::BUTTON_11},
	rHandFlag{ControllerBits::BUTTON_12},
	actionItems{},
	storageItems{},
	bodyOffset{bodyOffset_},
	handOffset{handOffset_}
{
	actionItems.resize(SLOT_COUNT);
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

	auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	// get base body position
	Vec2f basePos = getBodyPos();
	// calculate offset to hands
	Vec2f armOffset = Vec2f{ handOffset, 0.f };
	armOffset.angle(controller.stick2.angle());

	auto& leftItem = actionItems[0];
	if (!leftItem.tag.empty()) {
		leftItem.heldPos = basePos + armOffset;
		leftItem.angle = armOffset.angle();

		if (controller[lHandFlag] && leftItem.ability != nullptr) {
			leftItem.ability->doAbility(scene, id, controller, leftItem);
		}
	}

	auto& rightItem = actionItems[1];
	if (!rightItem.tag.empty()) {
		rightItem.heldPos = basePos - armOffset;
		rightItem.angle = armOffset.angle();

		if (controller[rHandFlag] && rightItem.ability != nullptr) {
			rightItem.ability->doAbility(scene, id, controller, rightItem);
		}
	}
}

void InventoryComponent::setActionItem(int slot, const Item& item) {
	// maybe crash?
	if (slot < 0 && slot > SLOT_COUNT - 1) return;

	actionItems[slot] = InventoryItem{ item.tag, 1, item.getAbility()};
}

Vec2f InventoryComponent::getBodyPos() const {
	auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	return physicsComp->position() + bodyOffset;
}

Vec2f InventoryComponent::getHandPos(int hand) {
	if (hand < 0 || hand > SLOT_COUNT - 1) return Vec2f{};
	return actionItems[hand].heldPos;
}

float InventoryComponent::getHandAngle(int hand) {
	if (hand < 0 || hand > SLOT_COUNT - 1) return 0.f;
	return actionItems[hand].angle;
}

