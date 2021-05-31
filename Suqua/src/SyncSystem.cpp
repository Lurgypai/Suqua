#include <algorithm>

#include "SyncSystem.h"
#include "NetworkDataComponent.h"
#include "OnlineComponent.h"
#include "ControllerComponent.h"

#include "Game.h"

void SyncSystem::storeCurrentState(Tick gameTime) {
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
	if (s != states.at(s.getGameTime())) {
        states.at(s.getGameTime()) = s;
        //clear states after time
        //you can't use remove_if with an associative container!
        for(auto iter = states.begin(); iter != states.end();) {
            if(iter->first > s.getGameTime())
                iter = states.erase(iter);
            else
                ++iter;
        }
        //apply state
        s.applyState();
        Tick currTick = game.getGameTick();
        game.setGameTick(s.getGameTime());
        while(game.getGameTick() < currTick) {
            game.physicsUpdate();
            storeCurrentState(game.getGameTick());
        } 
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
