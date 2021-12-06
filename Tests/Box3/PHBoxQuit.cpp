#include "PHBoxQuit.h"
#include "OnlineSystem.h"
#include "Game.h"
#include "EntityBaseComponent.h"
#include "ClientBoxScene.h"

PHBoxQuit::PHBoxQuit(PacketId id_) : PacketHandler{ id_ } {}

PHBoxQuit::~PHBoxQuit() {}

void PHBoxQuit::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
	PacketId id;
	data >> id;
	NetworkId targetNetId;
	data >> targetNetId;

	EntitySystem::GetComp<EntityBaseComponent>(game.online.getEntity(targetNetId))->isDead = true;
	std::cout << "Removed net id " << targetNetId << '\n';
}
