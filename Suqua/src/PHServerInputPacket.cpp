#include "PHServerInputPacket.h"
#include "Tick.h"
#include "Game.h"
#include <iostream>

PHServerInputPacket::PHServerInputPacket(PacketId id_) : PacketHandler{ id_ } {}

void PHServerInputPacket::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
	PacketId id;
	data >> id;

	Tick sourceTime;
	data >> sourceTime;

	if (sourceTime < game.getGameTick()) {
		std::cout << "received out of sync input packet from peer " << sourcePeer << ", packet was for time " << sourceTime << ", current time " << game.getGameTick() << ".\n";
		//ByteStream oosPacket;
		//oosPacket << Packet::OOSId;
		//game.host.bufferDataToChannel(sourcePeer, 0, oosPacket);
		
	}
	else {
		NetworkId netId;
		data >> netId;

		Controller cont;
		cont.unserialize(data);

		game.serverInputQueue.storeInput(sourceTime, netId, cont);
	}
}