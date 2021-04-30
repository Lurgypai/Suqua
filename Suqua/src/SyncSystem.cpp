#include "SyncSystem.h"
#include "NetworkDataComponent.h"
#include "OnlineComponent.h"
#include "ControllerComponent.h"

#include "Game.h"

void SyncSystem::storeCurrentState(Tick gameTime) {
	states.emplace(gameTime, gameTime);
}

void SyncSystem::writeStatePacket(ByteStream& stream, Tick gameTime)
{
	stream << Packet::StateId;
	states.at(gameTime).serialize(stream);
	
}

void SyncSystem::resyncStatePacket(ByteStream& stream, Game& game) {
	stream.setReadPos(0);
	SyncState s;
	s.unserialize(stream, game.online);
	if (s != states.at(s.getGameTime())) {

	}
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

