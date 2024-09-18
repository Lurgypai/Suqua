#include "LobbyScene.h"
#include "Game.h"
#include "../Shooty2Core/EntityGenerator.h"
#include "../Shooty2Core/Shooty2Packet.h"
#include <iostream>

LobbyScene::LobbyScene(SceneId id_, Scene::FlagType flags_, SceneId playingScene_, int minPlayerCount_) :
	Scene{id_, flags_},
	playingScene{playingScene_},
	minPlayerCount{minPlayerCount_},
	currPlayerCount{ 0 }
{
}

void LobbyScene::load(Game& game)
{
}

void LobbyScene::physicsStep(Game& game) {
	//std::cout << currPlayerCount << " / " << minPlayerCount << '\n';
	// if (currPlayerCount >= minPlayerCount) {
		// game.sceneOff(id);
		// game.sceneOn(playingScene);
		// std::cout << "Activated Playing Scene!\n";
	// }
}

void LobbyScene::renderUpdateStep(Game& game)
{
}

void LobbyScene::renderStep(Game& game)
{
}

void LobbyScene::unload(Game& game)
{
}

void LobbyScene::onConnect(Game& game, PeerId connectingId) {
	std::cout << "Peer " << connectingId << " connected.\n";
	++currPlayerCount;

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
}

void LobbyScene::onDisconnect(Game& game, PeerId disconnectedPeer) {
	std::cout << "Peer " << disconnectedPeer << " disconnected.\n";
	--currPlayerCount;
}
