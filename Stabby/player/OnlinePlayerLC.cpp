
#include "OnlinePlayerLC.h"
#include <iostream>
#include "SDL.h"
#include "PositionComponent.h"
#include "DirectionComponent.h"

#include "combat.h"

OnlinePlayerLC::OnlinePlayerLC(EntityId id_) :
	id{ id_ },
	previousPos{ {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} },
	whens{}
{
	if (id != 0) {
		if (!EntitySystem::Contains<PositionComponent>() || EntitySystem::GetComp<PositionComponent>(id) == nullptr) {
			EntitySystem::MakeComps<PositionComponent>(1, &id);
		}
		if (!EntitySystem::Contains<PlayerStateComponent>() || EntitySystem::GetComp<PlayerStateComponent>(id) == nullptr) {
			EntitySystem::MakeComps<PlayerStateComponent>(1, &id);
			EntitySystem::GetComp<PlayerStateComponent>(id)->playerState.facing = 1;
		}
		if (!EntitySystem::Contains<CombatComponent>() || EntitySystem::GetComp<CombatComponent>(id) == nullptr) {
			EntitySystem::MakeComps<CombatComponent>(1, &id);
		}
	}
}

EntityId OnlinePlayerLC::getId() const {
	return id;
}

void OnlinePlayerLC::interp(PlayerState st, Time_t when) {
	PositionComponent* position = EntitySystem::GetComp<PositionComponent>(id);
	PlayerStateComponent *playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
	DirectionComponent * dir = EntitySystem::GetComp<DirectionComponent>(id);
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(id);
	PlayerState & state = playerState->playerState;

	state = st;

	previousPos[0] = previousPos[1];
	previousPos[1] = previousPos[2];
	previousPos[2] = st.pos;

	whens[0] = whens[1];
	whens[1] = whens[2];
	whens[2] = when;

	state.pos = previousPos[0];
	position->pos = previousPos[0] - Vec2f{static_cast<float>(PlayerLC::PLAYER_WIDTH) / 2, static_cast<float>(PlayerLC::PLAYER_HEIGHT)};
	dir->dir = state.facing;

	combat->attack.setActive(state.activeAttack);
	combat->attack.setFrame(state.attackFrame);
	combat->attack.setSpeed(state.attackSpeed);
	combat->teamId = st.teamId;
}

void OnlinePlayerLC::update(Time_t gameTime) {
	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);

	//this is how long it is between each update.
	double delta = static_cast<double>(whens[1] - whens[0]) * GAME_TIME_STEP;

	Vec2f moveDistance = (previousPos[1] - previousPos[0]) * static_cast<float>(CLIENT_TIME_STEP / delta);
	position->pos += moveDistance;
}
