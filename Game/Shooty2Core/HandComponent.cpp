#include "HandComponent.h"
#include "ControllerComponent.h"
#include "PhysicsComponent.h"
#include "PositionComponent.h"

HandComponent::HandComponent(EntityId id_, Vec2f bodyOffset_, float handOffset_) :
	id{ id_ },
	handFlags{ ControllerBits::BUTTON_5, ControllerBits::BUTTON_6 },
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

void HandComponent::update(Scene& scene, float delta) {
	// update items
	for (int hand = 0; hand != actionItems.size(); ++hand) {
		auto& item = actionItems[hand];
        ItemAbility* ability = item.item.getAbility();
		if (ability != nullptr) ability->update(delta);
	}

	// use items
	auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);
	if (contComp == nullptr) return;
	const auto& controller = contComp->getController();

	// toggle between aligned and spread
	if (controller.toggled(ControllerBits::BUTTON_10)) {
		if (controller[ControllerBits::BUTTON_10]) handsAligned = !handsAligned;
	}

	// get base body position
	Vec2f basePos = getBodyPos();

	for (int i = 0; i != SLOT_COUNT; ++i) {
		auto& leftItem = actionItems[i];
		if (!leftItem.item.getTag().empty()) {
			handTargetPos[i] = getTargetHandPos(i, handsAligned, basePos, handOffset, controller.stick2.angle());
			// move hand and set angle
			Vec2f delta = handTargetPos[i] - leftItem.heldPos;
			leftItem.heldPos += delta * handMoveRate;
			leftItem.angle = controller.stick2.angle();
			// activate abilities
            ItemAbility* ability = leftItem.item.getAbility();
			if (controller[handFlags[i]] && ability != nullptr) {
				ability->doAbility(scene, id, leftItem.targetEntity, controller, leftItem.heldPos, leftItem.angle);
			}
		}
	}
}

void HandComponent::setItem(int slot, const Item& item) {
	setItem(slot, item, id);
}

void HandComponent::setItem(int slot, const Item& item, EntityId targetEntity) {
	// maybe crash?
	if (slot < 0 && slot > SLOT_COUNT - 1) return;

	actionItems[slot] = HandItem{ item, 1, targetEntity};
	auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);
	actionItems[slot].heldPos = getTargetHandPos(slot, handsAligned, getBodyPos(), handOffset,
		contComp->getController().stick2.angle());
}

Vec2f HandComponent::getBodyPos() const {
	auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	if(physicsComp) return physicsComp->position() + bodyOffset;
	auto* positionComp = EntitySystem::GetComp<PositionComponent>(id);
	return positionComp->pos + bodyOffset;
}

Vec2f HandComponent::getHandPos(int hand) const {
	if (hand < 0 || hand > SLOT_COUNT - 1) return Vec2f{};
	return actionItems[hand].heldPos;
}

float HandComponent::getHandAngle(int hand) const {
	if (hand < 0 || hand > SLOT_COUNT - 1) return 0.f;
	return actionItems[hand].angle;
}

const std::string& HandComponent::getHandTag(int hand) const {
	// more elegant crashes?
	return actionItems.at(hand).item.getTag();
}

bool HandComponent::handIsActive(int hand) const {
	if (hand < 0 || hand > SLOT_COUNT - 1) return false;
	return !actionItems[hand].item.getTag().empty();
}
