#include "PHServerPing.h"
#include "Game.h"

PHServerPing::PHServerPing(PacketId id_) :
	PacketHandler{ id_ }
{}

void PHServerPing::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
	//send it back with our time added
	data << game.getGameTick();
	game.host.bufferDataToChannel(sourcePeer, 0, data);

	std::cout << "Ping received at time" << game.getGameTick() << '\n';
}
