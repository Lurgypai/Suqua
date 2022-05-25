#include "LobbyScene.h"
#include "../Stabby2Core/PlayingScene.h"
#include "Game.h"
#include "../Stabby2Core/Stabby2Packet.h"
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

void LobbyScene::prePhysicsStep(Game& game)
{
}

void LobbyScene::physicsStep(Game& game) {
	//std::cout << currPlayerCount << " / " << minPlayerCount << '\n';
	if (currPlayerCount >= minPlayerCount) {
		game.sceneOff(id);
		game.sceneOn(playingScene);
		std::cout << "Activated Playing Scene!\n";
	}
}

void LobbyScene::postPhysicsStep(Game& game)
{
}

void LobbyScene::preRenderStep(Game& game)
{
}

void LobbyScene::renderStep(Game& game)
{
}

void LobbyScene::postRenderStep(Game& game)
{
}

void LobbyScene::unload(Game& game)
{
}

void LobbyScene::onConnect(Game& game, PeerId connectingId) {
	std::cout << "Peer " << connectingId << " connected.\n";
	++currPlayerCount;

	PlayingScene& ps = game.getScene<PlayingScene>(playingScene);
	EntityId newPlayerId = ps.addPlayer();
	game.online.addOnlineComponent(newPlayerId);
	OnlineComponent* onlineComp = EntitySystem::GetComp<OnlineComponent>(newPlayerId);
	game.host.addNetIdToPeer(connectingId, onlineComp->getNetId());

	ByteStream joinPacket{};
	joinPacket << Stabby2Packet::JoinPacket;
	joinPacket << onlineComp->getNetId();

	game.host.bufferAllDataByChannel(1, joinPacket);

	for (auto& online : EntitySystem::GetPool<OnlineComponent>()) {
		if (online.getNetId() != onlineComp->getNetId()) {
			ByteStream joinPacket2{};
			joinPacket2 << Stabby2Packet::JoinPacket;
			joinPacket2 << online.getNetId();

			game.host.bufferData(connectingId, joinPacket2);
		}
	}

	ByteStream ownedNetIdPacket{};
	ownedNetIdPacket << Stabby2Packet::OwnedNetIdPacket;
	ownedNetIdPacket << onlineComp->getNetId();

	game.host.bufferDataToChannel(connectingId, 1, ownedNetIdPacket);
}

void LobbyScene::onDisconnect(Game& game, PeerId disconnectedPeer) {
	std::cout << "Peer " << disconnectedPeer << " disconnected.\n";
	--currPlayerCount;
}
