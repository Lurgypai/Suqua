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

	NetworkId netId;
	data >> netId;

	Controller cont;
	cont.unserialize(data);

	game.serverInputQueue.storeInput(sourceTime, netId, cont);
}