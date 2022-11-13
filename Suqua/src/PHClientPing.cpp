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

	// set one frame prior to the expected next frame.
	// this ensures that the next input created and sent will be the one the server wants
	Tick newTime = arrivalTime - game.networkInputDelay - 1;
	game.setGameTick(newTime);
	std::cout << "Set time to " << newTime << '\n';
	//std::cout << "new time: " << arrivalTime + (rtt / 2) << '\n';
}
