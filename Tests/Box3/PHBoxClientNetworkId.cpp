#include "PHBoxClientNetworkId.h"
#include <iostream>
#include "Game.h"

PHBoxClientNetworkId::PHBoxClientNetworkId(PacketId id_, SceneId targetSceneId_) :
	PacketHandler{id_},
	targetSceneId{targetSceneId_}
{}

void PHBoxClientNetworkId::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
	PacketId id;
	data >> id;

	NetworkId netId;
	data >> netId;
	auto& targetScene = game.getScene<ClientBoxScene>(targetSceneId);
	targetScene.ourBoxNetId = netId;
	game.addOwnedNetId(netId);
	std::cout << "Set our network id to " << netId << '\n';
}
