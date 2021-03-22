#include "BoxComponent.h"
#include "NetworkDataComponent.h"
#include "PhysicsComponent.h"
#include "PositionData.h"
#include "ControllerComponent.h"
#include <SDL.h>

#include <iostream>

BoxComponent::BoxComponent(EntityId id_) :
	id{id_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<PhysicsComponent>() || !EntitySystem::GetComp<PhysicsComponent>(id)) {
			EntitySystem::MakeComps<PhysicsComponent>(1, &id);
		}
	}
}

EntityId BoxComponent::getId() const {
	return id;
}

void BoxComponent::update() {
	NetworkDataComponent* data = EntitySystem::GetComp<NetworkDataComponent>(id);
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	const Controller& controller = cont->getController();

	if (controller[ControllerBits::BUTTON_11]) {
		data->get<float>(X) = controller.pointerPos.x - (physics->getCollider().res.x / 2);
		data->get<float>(Y) = controller.pointerPos.y - (physics->getCollider().res.y / 2);

		data->get<float>(XVEL) = 0;
		data->get<float>(YVEL) = 0;
	}
}
