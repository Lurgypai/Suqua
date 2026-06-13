#include "DaemonComponent.h"
#include "ControllerComponent.h"
#include "HandComponent.h"
#include "PhysicsComponent.h"

DaemonComponent::DaemonComponent(EntityId id_, float followRate_, Vec2f followOffset_) :
	id{id_},
	followRate{followRate_},
	followOffset{followOffset_},
	hostEntity{0},
	isHolding{false},
	targetPos{},
	side{1}
{}

void DaemonComponent::update(double timeDelta) {
	auto* hostHands = EntitySystem::GetComp<HandComponent>(hostEntity);
	if (hostHands == nullptr) return;

	auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);
	if (contComp->getController().toggled(ControllerBits::BUTTON_9)) {
		// if toggled down, flip isHolding
		if(contComp->getController()[ControllerBits::BUTTON_9]) isHolding = !isHolding;
		if (isHolding) {
			targetPos = hostHands->getBodyPos();
		}
	}
	
	auto* physComp = EntitySystem::GetComp<PhysicsComponent>(hostEntity);
	if (physComp->vel.x > 0) {
		side = 1;
	}
	else if (physComp->vel.x < 0) {
		side = -1;
	}

	if (!isHolding) targetPos = hostHands->getBodyPos() + Vec2f{ followOffset.x * side, followOffset.y };

	auto* ourPhys = EntitySystem::GetComp<PhysicsComponent>(id);
	auto delta = targetPos - ourPhys->center();
	ourPhys->vel = (delta / timeDelta) * followRate;
}
