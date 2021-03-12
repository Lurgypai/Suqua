#include "BoxComponent.h"
#include "NetworkDataComponent.h"
#include "PhysicsComponent.h"
#include "PositionData.h"
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
	Vec2i mousePos;
	auto state = SDL_GetMouseState(&mousePos.x, &mousePos.y);
	if (state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		data->get<float>(X) = mousePos.x - (physics->getCollider().res.x / 2);
		data->get<float>(Y) = mousePos.y - (physics->getCollider().res.y / 2);

		data->get<float>(XVEL) = 0;
		data->get<float>(YVEL) = 0;
	}
}
