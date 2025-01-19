#include "TopDownMoverComponent.h"
#include "ControllerComponent.h"
#include "NetworkDataComponent.h"
#include "NetworkDataComponentDataFields.h"
#include "PhysicsComponent.h"

TopDownMoverComponent::TopDownMoverComponent(EntityId id_, float moveSpeed_) :
	id{ id_ },
	moveSpeed{nullptr}
{
    auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
    if(ndc == nullptr) throw std::runtime_error{"TopDownMoverComponent: NetworkDataComponent was not initialized."};
    ndc->set<float>(MovementData::MOVE_SPEED, moveSpeed_);
    moveSpeed = &ndc->get<float>(MovementData::MOVE_SPEED);
}

void TopDownMoverComponent::update() {
	auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);
	auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	if (contComp && physicsComp) {
		physicsComp->setVel(contComp->getController().stick1 * (*moveSpeed));
	}
}

float TopDownMoverComponent::getMoveSpeed() {
	return *moveSpeed;
}

void TopDownMoverComponent::setMoveSpeed(float moveSpeed_) {
	*moveSpeed = moveSpeed_;
}
