#include "pch.h"
#include "ServerPlayerLC.h"
#include <algorithm>
#include "ServerClientData.h"
#include "PhysicsAABB.h"
#include <iostream>

#include "SDL.h"
ServerPlayerLC::ServerPlayerLC(EntityId id_) :
	PlayerLC{id_},
	when{0},
	latest{},
	prevStates{}
{
	prevStates.emplace_back(PlayerState{});
}

ServerPlayerLC::ServerPlayerLC(const ServerPlayerLC & other) :
	PlayerLC{other},
	when{other.when},
	latest{other.latest},
	prevStates{other.prevStates}
{}

Time_t ServerPlayerLC::getWhen() const {
	return when;
}

PlayerState ServerPlayerLC::getStateAt(Time_t gameTime) {
	for (auto iter = prevStates.rbegin(); iter != prevStates.rend(); ++iter) {
		if (iter->when < gameTime)
			if (iter != prevStates.rbegin())
				return(*iter--);
			else
				return *iter;
	}
	return prevStates.front();
}

void ServerPlayerLC::setWhen(Time_t when_) {
	when = when_;
}

void ServerPlayerLC::bufferInput(ClientCommand c) {
	if (c.when > latest.when) {
		latest = c;
		when = latest.when - 1; // move back a single client tick. Update will increment client time while we wait for our next input.
	}
}

void ServerPlayerLC::update(Time_t gameTime, const Stage& stage) {
	//we need to increment when as time passes, so that the client knows the times when things should happen. Update runs at client speed.
	++when;
	PlayerLC::update(CLIENT_TIME_STEP, latest.controllerState, stage);

	if (prevStates.size() >= 32)
		prevStates.pop_front();
	prevStates.emplace_back(PlayerState{state, gameTime, pos, vel, rollFrame, attack.getActiveId(), attack.getCurrFrame(), health, stunFrame});
}

//check if we hit anyone else from ze past
void ServerPlayerLC::runHitDetect(Time_t gameTime) {
	Time_t timeToFind = gameTime - (.1 * GAME_TIME_STEP);
	PlayerState stateWhenAction = getStateAt(timeToFind);

	if (stateWhenAction.state != State::rolling) {
		Vec2f restorePos = collider.getPos();

		collider.setPos(stateWhenAction.pos);

		bool wasHit = false;
		for (auto& playerComp : *EntitySystem::GetPool<ServerPlayerLC>()) {
			if (!playerComp.isFree && playerComp.val.id != id) {
				auto& player = playerComp.val;
				if (player.getAttack().getActive() != nullptr && collider.intersects(player.getAttack().getActive()->hit)) {
					if (!isBeingHit) {
						damage(1);
					}
					wasHit = true;
				}
			}
		}
		isBeingHit = wasHit;

		collider.setPos(restorePos);
	}
}
