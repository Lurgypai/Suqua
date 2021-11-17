#include <algorithm>

#include "SyncSystem.h"
#include "NetworkDataComponent.h"
#include "OnlineComponent.h"
#include "ControllerComponent.h"

#include "Game.h"

void SyncSystem::storeCurrentState(Tick gameTime) {
    //properly rotate out states
    auto toRemove = states.find(gameTime - (120 * 5));
    if (toRemove != states.end())
        states.erase(toRemove);

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
}

const std::unordered_map<Tick, SyncState>& SyncSystem::getStates() const {
    return states;
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
