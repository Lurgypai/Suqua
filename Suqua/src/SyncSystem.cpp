#include <algorithm>
#include <iostream>

#include "SyncSystem.h"
#include "NetworkDataComponent.h"
#include "OnlineComponent.h"
#include "ControllerComponent.h"

#include "Game.h"

void SyncSystem::storeCurrentState(Tick gameTime) {
    //properly rotate out states
    auto toRemove = states.find(gameTime - (120 * 5));
    if (toRemove != states.end()) states.erase(toRemove);

	states.emplace(gameTime, gameTime);
}

void SyncSystem::overrideState(Tick gameTime) {
    states.at(gameTime) = SyncState{gameTime};
}

void SyncSystem::writeStatePacket(ByteStream& stream, Tick gameTime)
{
	stream << Packet::StateId;
	states.at(gameTime).serialize(stream);
}

void SyncSystem::resyncStatePacket(ByteStream& stream, Game& game) {
    //create dummy state
	SyncState s{0};
	s.unserialize(stream, game.online);
    //TODO: Add a better error handler for this, or make sure it doesn't happen in the first place
    if (states.find(s.getGameTime()) == states.end())
        return;
	if (s != states.at(s.getGameTime())) {
        std::cout << "Resynchronizing for time " << s.getGameTime() << '\n';
        states.at(s.getGameTime()) = s;
        //clear states after time
        //you can't use remove_if with an associative container!
        std::unordered_map<Tick, SyncState> removedStates{};
        for(auto iter = states.begin(); iter != states.end();) {
            if (iter->first > s.getGameTime()) {
                removedStates.emplace(std::move(*iter));
                iter = states.erase(iter);
            }
            else ++iter;
        }
        SyncState currState{ game.getGameTick() };
        //apply state
        s.applyState();
        Tick currTick = game.getGameTick();
        game.setGameTick(s.getGameTime());
        while(game.getGameTick() < currTick - 1) {
            removedStates.at(game.getGameTick() + 1).applyInput(game.getOwnedNetIds(), game.online);
            game.physicsUpdate();
            game.tickTime();
            storeCurrentState(game.getGameTick());
        }
        //the state for the current frame has not yet been stored (waiting till after all packets are processed)
        currState.applyInput(game.getOwnedNetIds(), game.online);
        game.physicsUpdate();
        game.tickTime();
	}

    //acknowledge it after resolved
    states.at(s.getGameTime()).setServerAcknowledged();
    interpolate(game.getGameTick() - 2 * game.serverBroadcastDelay);
    //std::cout << "Acknowledged state for time " << s.getGameTime() << '\n';
}

const std::unordered_map<Tick, SyncState>& SyncSystem::getStates() const {
    return states;
}

bool SyncSystem::hasState(Tick gameTime) const {
    return states.find(gameTime) != states.end();
}

void SyncSystem::interpolate(Tick gameTime) {
    //find the most recent acknowledged state (including this)
    //std::cout << "Looking to interpolate: " << gameTime << '\n';
    Tick prevTick;
    for (Tick offset = 0;; ++offset) {
        auto target = states.find(gameTime - offset);
        if (target == states.end()) return; // if it doesn't exist (we just joined probably)
        if (target->second.isServerAcknowledged()) {
            prevTick = gameTime - offset;
            break;
        }
    }
    //std::cout << "Found previous state\n";

    //find the next acknowledged state (excluding this)
    Tick nextTick;
    for (Tick offset = 1;; ++offset) {
        auto target = states.find(gameTime + offset);
        if (target == states.end()) return; // if it doesn't exist (we just joined probably)
        if (target->second.isServerAcknowledged()) {
            nextTick = gameTime + offset;
            break;
        }
    }
    //std::cout << "Found next state\n";

    auto& prevState = states.at(prevTick);
    auto& nextState = states.at(nextTick);

    //std::cout << "prevTick: " << prevTick << ", currTick: " << gameTime << ", nextTick: " << nextTick << '\n';
    //std::cout << "Interpolation level " << static_cast<float>(gameTime - prevTick) / (nextTick - prevTick) << '\n';
    prevState.interp(nextState, static_cast<float>(gameTime - prevTick) / (nextTick - prevTick));

    //to be tested
}

//store the state as set of all ndc/controller in id addressable slot
//serialize when needed
//unserialze gamestate
/*
	resync ->
		unserialize game state
		compare at time
		if incorrect set, reupdate
*/
//properly synchromize time
