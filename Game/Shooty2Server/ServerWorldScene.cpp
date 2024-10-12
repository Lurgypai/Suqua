#include "ServerWorldScene.h"
#include "Game.h"
#include "../Shooty2Core/Shooty2Packet.h"
#include <iostream>
#include "PHServerRequestPlayerNetId.h"
#include "PHServerState.h"

ServerWorldScene::ServerWorldScene(SceneId id_, Scene::FlagType flags_) :
	Scene{id_, flags_},
	currPlayerCount{ 0 }
{
}

void ServerWorldScene::load(Game& game)
{
    game.loadPacketHandler<PHServerRequestPlayerNetId>(Shooty2Packet::RequestPlayerNetId);
    game.loadPacketHandler<PHServerState>(Packet::StateId);
}

void ServerWorldScene::physicsStep(Game& game) {
	//std::cout << currPlayerCount << " / " << minPlayerCount << '\n';
	// if (currPlayerCount >= minPlayerCount) {
		// game.sceneOff(id);
		// game.sceneOn(playingScene);
		// std::cout << "Activated Playing Scene!\n";
	// }
}

void ServerWorldScene::renderUpdateStep(Game& game)
{
}

void ServerWorldScene::renderStep(Game& game)
{
}

void ServerWorldScene::unload(Game& game)
{
}

void ServerWorldScene::onConnect(Game& game, PeerId connectingId) {
	std::cout << "Peer " << connectingId << " connected.\n";
	++currPlayerCount;

    /*
    EntityId newPlayerId = EntityGenerator::SpawnPlayerPuppet(*this)[0];
	game.online.addOnlineComponent(newPlayerId);
	OnlineComponent* onlineComp = EntitySystem::GetComp<OnlineComponent>(newPlayerId);

	ByteStream joinPacket{};
	joinPacket << Shooty2Packet::JoinPacket;
	joinPacket << onlineComp->getNetId();

	game.host.bufferAllDataByChannel(1, joinPacket);

	for (auto& online : EntitySystem::GetPool<OnlineComponent>()) {
		if (online.getNetId() != onlineComp->getNetId()) {
			ByteStream joinPacket2{};
			joinPacket2 << Shooty2Packet::JoinPacket;
			joinPacket2 << online.getNetId();

			game.host.bufferData(connectingId, joinPacket2);
		}
	}
    */
}

void ServerWorldScene::onDisconnect(Game& game, PeerId disconnectedPeer) {
	std::cout << "Peer " << disconnectedPeer << " disconnected.\n";
	--currPlayerCount;
}
