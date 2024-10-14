#include "ServerWorldScene.h"
#include "Game.h"
#include "../Shooty2Core/Shooty2Packet.h"
#include <iostream>
#include "PHServerSpawnEntities.h"
#include "PHServerState.h"
#include "PHServerDeadEntities.h"
#include "NetworkEntityOwnershipSystem.h"

#include "../Shooty2Core/EntityGenerator.h"
#include "../Shooty2Core/EntitySpawnSystem.h"
#include "Packet.h"

ServerWorldScene::ServerWorldScene(SceneId id_, Scene::FlagType flags_) :
	Scene{id_, flags_},
	currPlayerCount{ 0 }
{
}

void ServerWorldScene::load(Game& game)
{

    EntitySpawnSystem::Init<EntityGenerator>();

    game.loadPacketHandler<PHServerSpawnEntities>(Shooty2Packet::SpawnEntities);
    game.loadPacketHandler<PHServerState>(Packet::StateId);
    game.loadPacketHandler<PHServerDeadEntities>(Packet::DeadEntities);
}

void ServerWorldScene::physicsStep(Game& game) {
	//std::cout << currPlayerCount << " / " << minPlayerCount << '\n';
	// if (currPlayerCount >= minPlayerCount) {
		// game.sceneOff(id);
		// game.sceneOn(playingScene);
		// std::cout << "Activated Playing Scene!\n";
	// }
    broadcastDeadEntities(game.host);
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

void ServerWorldScene::onConnect(Game& game, PeerId connectingPeer) {
	++currPlayerCount;
    std::cout << "Peer " << connectingPeer << " connection received."
        " Sending existing entities.\n";
    ByteStream spawnPacket;
    spawnPacket << Shooty2Packet::SpawnEntities;
    for(const auto& [peer, entities] : game.networkEntityOwnershipSystem.getOwnedEntities()) {
        for(const auto& entity : entities) {
            spawnPacket << entity.tag;
            spawnPacket << Vec2f{0.f, 0.f};
            spawnPacket << entity.netIds[0];
            spawnPacket << NetworkOwnerComponent::Owner::foreign;
            for(int i = 1; i != entity.netIds.size(); ++i) {
                spawnPacket << entity.netIds[i];
            }
        }
    }

    game.host.bufferDataToChannel(connectingPeer, 0, spawnPacket);
}

void ServerWorldScene::onDisconnect(Game& game, PeerId disconnectedPeer) {
	std::cout << "Peer " << disconnectedPeer << " disconnected.\n";
	--currPlayerCount;

    const auto& owned = game.networkEntityOwnershipSystem.getOwnedEntities();
    auto disconnectedEntities = owned.find(disconnectedPeer);
    if(disconnectedEntities == owned.end()) return;

    ByteStream dead;
    dead << Packet::DeadEntities;
    for(const auto& entity : disconnectedEntities->second) {
        for(auto netId : entity.netIds) {
            dead << netId;
        }
    }

    game.host.bufferAllDataByChannel(0, dead);
}
