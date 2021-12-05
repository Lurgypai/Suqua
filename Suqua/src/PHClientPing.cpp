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
	//set to just arrival time - 1, ensures 100% consistency
	game.setGameTick(arrivalTime - 1);
	std::cout << "Set time to " << arrivalTime - 1 << '\n';
	//std::cout << "new time: " << arrivalTime + (rtt / 2) << '\n';
}
