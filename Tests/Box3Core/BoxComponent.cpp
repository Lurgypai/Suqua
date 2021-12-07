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

		auto* data = EntitySystem::GetComp<NetworkDataComponent>(id);
		data->set<float>(PREV_X, 0);
		data->set<float>(PREV_Y, 0);
	}
}

EntityId BoxComponent::getId() const {
	return id;
}

void BoxComponent::update(const Game& game) {
	NetworkDataComponent* data = EntitySystem::GetComp<NetworkDataComponent>(id);
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	const Controller& controller = cont->getController();
	
	constexpr float MOVE_RATIO = 0.08;
	if (controller[ControllerBits::BUTTON_11]) {
		Vec2f currPos = { data->get<float>(X), data->get<float>(Y) };
		data->get<float>(PREV_X) = currPos.x;
		data->get<float>(PREV_Y) = currPos.y;

		Vec2f targetPos = { controller.pointerPos.x - (physics->getCollider().res.x / 2) , controller.pointerPos.y - (physics->getCollider().res.y / 2) };
		currPos += (targetPos - currPos) * MOVE_RATIO;

		data->get<float>(X) = currPos.x;
		data->get<float>(Y) = currPos.y;

		float newXVel = (currPos.x - data->get<float>(PREV_X)) / game.PHYSICS_STEP;
		float newYVel = (currPos.y - data->get<float>(PREV_Y)) / game.PHYSICS_STEP;

		data->get<float>(XVEL) = newXVel;
		data->get<float>(YVEL) = newYVel;
	}


	if (data->get<bool>(GROUNDED)) data->get<float>(XVEL) = 0;
	data->get<bool>(COLLIDEABLE) = data->get<bool>(GROUNDED);
}
