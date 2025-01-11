#include "TopDownMoverComponent.h"
#include "ControllerComponent.h"
#include "NetworkDataComponent.h"
#include "NetworkDataComponentDataFields.h"
#include "PhysicsComponent.h"

TopDownMoverComponent::TopDownMoverComponent(EntityId id_) :
	id{ id_ },
	moveSpeed{nullptr}
{
	if (id != 0) {
		if (!EntitySystem::Contains<NetworkDataComponent>() || !EntitySystem::GetComp<NetworkDataComponent>(id)) {
			EntitySystem::MakeComps<NetworkDataComponent>(1, &id);
		}

		// generate the field
		auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
		ndc->set<float>(MovementData::MOVE_SPEED, 0.0f);
		moveSpeed = &ndc->get<float>(MovementData::MOVE_SPEED);
	}
}

EntityId TopDownMoverComponent::getId() const {
	return id;
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
