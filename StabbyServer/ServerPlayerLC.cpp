#include "pch.h"
#include "ServerPlayerLC.h"
#include <algorithm>
#include "ServerClientData.h"
#include "PhysicsAABBLC.h"
#include <iostream>

#include "SDL.h"
ServerPlayerLC::ServerPlayerLC(EntityId id_) :
	PlayerLC{id_},
	when{0},
	inputs{},
	prevStates{}
{
	prevStates.emplace_back(PlayerState{});
	inputs.reserve(INPUT_QUEUE_SIZE);
}

ServerPlayerLC::ServerPlayerLC(const ServerPlayerLC & other) :
	PlayerLC{other},
	when{other.when},
	inputs{other.inputs},
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
	if (inputs.size() < INPUT_QUEUE_SIZE);
	inputs.push_back(c);
}

void ServerPlayerLC::update(Time_t gameTime) {
	std::sort(inputs.begin(), inputs.end(), [](ClientCommand & first, ClientCommand & second) {return first.when < second.when;});
	for (auto input = inputs.begin(); input != inputs.end(); ++input) {
		when = input->when;
		
		PlayerLC::update(CLIENT_TIME_STEP, input->controllerState);

		if (prevStates.size() >= 32)
			prevStates.pop_front();
		prevStates.emplace_back(PlayerState{state, gameTime, pos, vel, rollFrame, attack.getActiveId(), attack.getCurrFrame(), health, stunFrame});
	}

	inputs.clear();
	inputs.reserve(INPUT_QUEUE_SIZE);
}

//check if we hit anyone else from ze past
void ServerPlayerLC::runHitDetect(Time_t gameTime) {
	Time_t timeToFind = gameTime - (.1 * GAME_TIME_STEP);
	PlayerState stateWhenAction = getStateAt(timeToFind);

	if (stateWhenAction.state != State::rolling) {
		PhysicsAABBLC * ourCollider = EntitySystem::GetComp<PhysicsAABBLC>(id);
		Vec2f restorePos = ourCollider->getPos();

		ourCollider->setPos(stateWhenAction.pos);

		if (ourCollider != nullptr) {
			bool wasHit = false;
			for (auto& playerComp : *EntitySystem::GetPool<ServerPlayerLC>()) {
				if (!playerComp.isFree && playerComp.val.id != id) {
					auto& player = playerComp.val;
					if (player.getAttack().getActive() != nullptr && ourCollider->intersects(player.getAttack().getActive()->hit)) {
						if (!isBeingHit) {
							damage(1);
						}
						wasHit = true;
					}
				}
			}
			isBeingHit = wasHit;
		}

		ourCollider->setPos(restorePos);
	}
}
