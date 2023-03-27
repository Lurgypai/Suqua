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

	Tick arrivalTime;
	data >> arrivalTime;

	if (sourceTime <= game.getGameTick()) {
		Tick rtt = game.getGameTick() - sourceTime;
		// set one frame prior to the expected next frame.
		// this ensures that the next input created and sent will be the one the server wants
		Tick newTime = arrivalTime + (rtt / 2) - 1;
		game.setGameTick(newTime);
		std::cout << "source time was " << sourceTime << ", arrival time was " << arrivalTime << ", rtt was " << rtt << ", setting time to " << newTime << '\n';
		//std::cout << "new time: " << arrivalTime + (rtt / 2) << '\n';
	}
}
