#include "..\header\PHGameTime.h"
#include "Game.h"

PHGameTime::PHGameTime(PacketHandlerId id_) :
	PacketHandler{id_}
{}

void PHGameTime::handlePacket(ByteStream& data, Game& game) {
	Tick time;
	data >> time;
	game.setGameTick(time);
	std::cout << "new time: " << time << '\n';
}
