#include "PHOOSPacket.h"
#include "Tick.h"
#include "Game.h"
#include <iostream>

PHOOSPacket::PHOOSPacket(PacketId id_) : PacketHandler{ id_ } {}

void PHOOSPacket::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
	std::cout << "Client notified it was out of sync, pinging server...\n";
	// game.pingConnected();
}