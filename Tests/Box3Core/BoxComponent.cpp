#include "BoxComponent.h"
#include "NetworkDataComponent.h"
#include "PhysicsComponent.h"
#include "PositionData.h"
#include "ControllerComponent.h"
#include "OnlineSystem.h"
#include "OnlineComponent.h"
#include <SDL.h>

#include <iostream>

BoxComponent::BoxComponent(EntityId id_) :
	id{id_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<PhysicsComponent>() || !EntitySystem::GetComp<PhysicsComponent>(id)) {
			EntitySystem::MakeComps<PhysicsComponent>(1, &id);
		}
		if (!EntitySystem::Contains<OnlineComponent>() || !EntitySystem::GetComp<OnlineComponent>(id)) {
			EntitySystem::MakeComps<OnlineComponent>(1, &id);
		}
	}
}

EntityId BoxComponent::getId() const {
	return id;
}

void BoxComponent::update() {
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	const Controller& controller = cont->getController();

	if (controller[ControllerBits::BUTTON_11]) {
		physics->setPos({
			controller.pointerPos.x - (physics->getCollider().res.x / 2),
			controller.pointerPos.y - (physics->getCollider().res.y / 2)
			});

		physics->setVel({ 0, 0 });
	}
}
