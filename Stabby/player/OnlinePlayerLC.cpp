#include <iostream>

#include "SDL.h"
#include "OnlinePlayerLC.h"
#include "PositionComponent.h"

#include "combat.h"

OnlinePlayerLC::OnlinePlayerLC(EntityId id_) :
	id{ id_ },
	previousPos{ {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} },
	whens{}
{
	if (id != 0) {
		if (!EntitySystem::Contains<PlayerLC>() || EntitySystem::GetComp<PlayerLC>(id) == nullptr) {
			EntitySystem::MakeComps<PlayerLC>(1, &id);
		}
	}
}

EntityId OnlinePlayerLC::getId() const {
	return id;
}

void OnlinePlayerLC::interp(PlayerState st, Time_t when) {
	//physics aren't applied
	st.frozen = true;
	PlayerLC* player = EntitySystem::GetComp<PlayerLC>(id);

	previousPos[0] = previousPos[1];
	previousPos[1] = previousPos[2];
	previousPos[2] = st.pos;

	whens[0] = whens[1];
	whens[1] = whens[2];
	whens[2] = when;

	st.pos = previousPos[0];
	player->setState(st);
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	//std::cout << "interping\n";

	Vec2f minPos = { std::fminf(previousPos[0].x, previousPos[1].x), std::fminf(previousPos[0].y, previousPos[1].y) };
	Vec2f maxPos = { std::fmaxf(previousPos[0].x, previousPos[1].x), std::fmaxf(previousPos[0].y, previousPos[1].y) };
	positionBox = { minPos, maxPos - minPos };
}

void OnlinePlayerLC::update(Time_t gameTime) {
	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);

	//this is how long it is between each update.
	double delta = static_cast<double>(whens[1] - whens[0]) * GAME_TIME_STEP;

	Vec2f moveDistance = (previousPos[1] - previousPos[0]) * static_cast<float>(CLIENT_TIME_STEP / delta);
	Vec2f newPos = position->pos + moveDistance;
	if (positionBox.contains(newPos))
		position->pos = newPos;
	//std::cout << "updating\n";
}

//its not moving now? I think. we'll see. you made change...