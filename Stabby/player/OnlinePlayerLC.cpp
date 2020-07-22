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

void OnlinePlayerLC::interp(PlayerState state, const DynamicBitset& changedFields, Time_t when) {

	PlayerLC* player = EntitySystem::GetComp<PlayerLC>(id);
	const auto& plrState = player->getState();

	if (!changedFields[2])
		state.state = plrState.state;
	if (!changedFields[3])
		state.rollFrame = plrState.rollFrame;
	if (!changedFields[4])
		state.activeAttack = plrState.activeAttack;
	if (!changedFields[5])
		state.attackFrame = plrState.attackFrame;
	if (!changedFields[6])
		state.health = plrState.health;
	if (!changedFields[7])
		state.stunFrame = plrState.stunFrame;
	if (!changedFields[8])
		state.healFrame = plrState.healFrame;
	if (!changedFields[9])
		state.healDelay = plrState.healDelay;
	if (!changedFields[10])
		state.facing = plrState.facing;
	if (!changedFields[11])
		state.spawnPoint = plrState.spawnPoint;
	if (!changedFields[12])
		state.attackFreezeFrame = plrState.attackFreezeFrame;
	if (!changedFields[13])
		state.teamId = plrState.teamId;
	if (!changedFields[14])
		state.stamina = plrState.stamina;
	if (!changedFields[15])
		state.staminaRechargeFrame = plrState.staminaRechargeFrame;
	if (!changedFields[16])
		state.deathFrame = plrState.deathFrame;
	if (!changedFields[17])
		state.attackSpeed = plrState.attackSpeed;
	if (!changedFields[18])
		state.moveSpeed = plrState.moveSpeed;
	if (!changedFields[19])
		state.weaponTag = plrState.weaponTag;


	//physics aren't applied
	state.frozen = true;


	previousPos[0] = previousPos[1];
	previousPos[1] = previousPos[2];
	//if we received a new position, update the furthest previousPos to it
	if (changedFields[20]) {
		previousPos[2] = state.pos;
	}

	whens[0] = whens[1];
	whens[1] = whens[2];
	whens[2] = when;

	state.pos = previousPos[0];
	player->setState(state);
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