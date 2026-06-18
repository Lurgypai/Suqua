#include "TopDownMoverComponent.h"
#include "ControllerComponent.h"
#include "PhysicsComponent.h"

TopDownMoverComponent::TopDownMoverComponent(EntityId id_, float moveSpeed_) :
	id{ id_ },
	moveSpeed{moveSpeed_},
	paused{false}
{}

void TopDownMoverComponent::update() {
	if (paused) return;
	auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);
	auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	if (contComp && physicsComp) {
		physicsComp->vel = contComp->getController().stick1 * moveSpeed;
	}
}

float TopDownMoverComponent::getMoveSpeed() {
	return moveSpeed;
}

void TopDownMoverComponent::setMoveSpeed(float moveSpeed_) {
	moveSpeed = moveSpeed_;
}
