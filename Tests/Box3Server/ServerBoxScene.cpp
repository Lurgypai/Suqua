#include "ServerBoxScene.h"
#include "Game.h"
#include "../Box3Core/BoxPacket.h"
#include "ByteStream.h"
#include "OnlineComponent.h"

ServerBoxScene::ServerBoxScene(SceneId id_, Scene::FlagType flags) :
	BoxScene{id_, flags}
{}

void ServerBoxScene::onConnect(Game& game, PeerId connectingId) {
	EntityId boxId = addBox(game, false, 0);
	NetworkId boxNetId = EntitySystem::GetComp<OnlineComponent>(boxId)->getNetId();
	game.host.addNetIdToPeer(connectingId, boxNetId);

	ByteStream packet{};
	packet << BoxPacket::ClientNetworkIdPacket;
	packet << boxNetId;
	game.host.bufferDataToChannel(connectingId, 0, packet);

	//send us a join packet for every other network id
	for (auto& online : EntitySystem::GetPool<OnlineComponent>()) {
		if (online.getId() != boxId) {
			ByteStream joinPacket{};
			joinPacket << BoxPacket::JoinPacket;
			joinPacket << online.getNetId();
			std::cout << "Notifying connected client of player " << online.getNetId() << '\n';
			game.host.bufferDataToChannel(connectingId, 0, joinPacket);
		}
	}

	//tell everyone that we've joined (including us)
	std::cout << "Broadcasting player join...\n";
	ByteStream joinPacket2{};
	joinPacket2 << BoxPacket::JoinPacket;
	joinPacket2 << boxNetId;
	game.host.bufferAllDataByChannel(0, joinPacket2);
}