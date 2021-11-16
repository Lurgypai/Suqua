#include "PHClientPing.h"
#include "Game.h"

PHClientPing::PHClientPing(PacketId id_) :
	PacketHandler{id_}
{}

void PHClientPing::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
	PacketId id;
	data >> id;

	Tick sourceTime;
	data >> sourceTime;
	Tick rtt = game.getGameTick() - sourceTime;
	Tick arrivalTime;
	data >> arrivalTime;
	game.setGameTick(arrivalTime + (rtt / 2));
	std::cout << "new time: " << arrivalTime + (rtt / 2) << '\n';
}
