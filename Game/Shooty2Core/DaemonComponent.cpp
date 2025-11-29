#include "DaemonComponent.h"
#include "PositionComponent.h"
#include "ControllerComponent.h"
#include "InventoryComponent.h"
#include "PhysicsComponent.h"

#include <print>

DaemonComponent::DaemonComponent(EntityId id_, float followRate_, Vec2f followOffset_) :
	id{id_},
	followRate{followRate_},
	followOffset{followOffset_},
	hostEntity{0},
	isHolding{false},
	targetPos{},
	side{1}
{}

void DaemonComponent::update() {
	auto* hostInv = EntitySystem::GetComp<InventoryComponent>(hostEntity);
	if (hostInv == nullptr) return;

	auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);
	if (contComp->getController().toggled(ControllerBits::BUTTON_7)) {
		// if toggled down, flip isHolding
		if(contComp->getController()[ControllerBits::BUTTON_7]) isHolding = !isHolding;
		if (isHolding) {
			targetPos = hostInv->getBodyPos();
		}
	}
	
	auto* physComp = EntitySystem::GetComp<PhysicsComponent>(hostEntity);
	if (physComp->vel.x > 0) {
		side = 1;
	}
	else if (physComp->vel.x < 0) {
		side = -1;
	}

	if (!isHolding) targetPos = hostInv->getBodyPos() + Vec2f{ followOffset.x * side, followOffset.y };

	auto* ourPos = EntitySystem::GetComp<PositionComponent>(id);
	auto delta = targetPos - ourPos->pos;
	ourPos->pos += delta * followRate;
}