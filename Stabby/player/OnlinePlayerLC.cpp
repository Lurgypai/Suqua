
#include "OnlinePlayerLC.h"
#include <iostream>
#include "SDL.h"
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
	PlayerLC* player = EntitySystem::GetComp<PlayerLC>(id);

	previousPos[0] = previousPos[1];
	previousPos[1] = previousPos[2];
	previousPos[2] = st.pos;

	whens[0] = whens[1];
	whens[1] = whens[2];
	whens[2] = when;

	st.pos = previousPos[0];
	player->setState(st);
}

void OnlinePlayerLC::update(Time_t gameTime) {
	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);

	//this is how long it is between each update.
	double delta = static_cast<double>(whens[1] - whens[0]) * GAME_TIME_STEP;

	Vec2f moveDistance = (previousPos[1] - previousPos[0]) * static_cast<float>(CLIENT_TIME_STEP / delta);
	position->pos += moveDistance;
}
