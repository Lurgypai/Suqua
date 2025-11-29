#include "DaemonComponent.h"
#include "PositionComponent.h"
#include "ControllerComponent.h"
#include "InventoryComponent.h"

#include <print>

DaemonComponent::DaemonComponent(EntityId id_, float followRate_, Vec2f followOffset_) :
	id{id_},
	followRate{followRate_},
	followOffset{followOffset_},
	hostEntity{0},
	isHolding{false},
	targetPos{}
{}

void DaemonComponent::update() {
	auto* hostInv = EntitySystem::GetComp<InventoryComponent>(hostEntity);
	if (hostInv == nullptr) return;

	auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);
	if (contComp->getController().toggled(ControllerBits::BUTTON_8)) {
		// if pressed, toggle
		if(contComp->getController()[ControllerBits::BUTTON_8]) isHolding = !isHolding;
		if (isHolding) {
			targetPos = hostInv->getBodyPos();
		}
	}

	if (!isHolding) targetPos = hostInv->getBodyPos() + followOffset;

	auto* ourPos = EntitySystem::GetComp<PositionComponent>(id);
	auto delta = targetPos - ourPos->pos;
	ourPos->pos += delta * followRate;
}