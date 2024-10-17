#include "ServerWorldScene.h"
#include "Game.h"
#include "../Shooty2Core/Shooty2Packet.h"
#include <iostream>
#include "PHServerSpawnEntities.h"
#include "PHServerState.h"
#include "PHServerDeadEntities.h"
#include "NetworkEntityOwnershipSystem.h"
#include "Updater.h"
#include "PositionComponent.h"
#include "EntityBaseComponent.h"

#include "../Shooty2Core/EntitySpawnSystem.h"
#include "Packet.h"
#include "AITopDownBasic.h"

#include "TopDownMoverComponent.h"
#include "ParentComponent.h"
#include "AimToLStickComponent.h"
#include "../Shooty2Core/GunFireComponent.h"
#include "LifeTimeComponent.h"
#include "../Shooty2Core/HealthWatcherComponent.h"
#include "../Shooty2Core/RespawnComponent.h"
#include "ServerEntityGenerator.h"

ServerWorldScene::ServerWorldScene(SceneId id_, Scene::FlagType flags_) :
	Scene{id_, flags_},
	currPlayerCount{ 0 }
{
}

void ServerWorldScene::load(Game& game)
{

    EntitySpawnSystem::Init<ServerEntityGenerator>(&game);

    game.loadPacketHandler<PHServerSpawnEntities>(Shooty2Packet::SpawnEntities, this);
    game.loadPacketHandler<PHServerState>(Packet::StateId);
    game.loadPacketHandler<PHServerDeadEntities>(Packet::DeadEntities);
 
	// dummy ai
	auto dummyEntities = EntitySpawnSystem::SpawnEntity("enemy.basic", *this, { 720.f / 2, 405.f / 2 }, NetworkOwnerComponent::Owner::local, true);
	dummy = dummyEntities[0];

	auto dummyAI = game.loadInputDevice<AITopDownBasic>();
	addEntityInputs({ { dummy, dummyAI } });
	auto& ai = static_cast<AITopDownBasic&>(game.getInputDevice(dummyAI));
	ai.entityId = dummy;
	ai.setTargetTeams({ TeamComponent::TeamId::player });
}

void ServerWorldScene::physicsStep(Game& game) {
	Updater::UpdateOwned<TopDownMoverComponent>();
	Updater::UpdateOwned<ParentComponent>();
	Updater::UpdateOwned<AimToLStickComponent>();
	Updater::UpdateOwned<GunFireComponent>(this);
	Updater::UpdateOwned<LifeTimeComponent>();
	Updater::UpdateOwned<HealthWatcherComponent>();
	Updater::UpdateOwned<RespawnComponent>();

	physics.runPhysicsOnOwned(game.PHYSICS_STEP);

    broadcastDeadEntities(game);
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
        if(peer == connectingPeer) continue;

        for(const auto& entity : entities) {
            std::cout << "Sending spawn request for tag " << entity.tag << '\n';
            spawnPacket << entity.tag;
            spawnPacket << Vec2f{0.f, 0.f};
            spawnPacket << NetworkOwnerComponent::Owner::foreign;
            for(const auto & netId : entity.netIds) {
                spawnPacket << netId;
            }
        }
    }

    for(const auto& entityDescriptor : game.networkEntityOwnershipSystem.getLocalEntities()) {
        std::cout << "Sending spawn request for tag " << entityDescriptor.tag << '\n';
        spawnPacket<< entityDescriptor.tag;
        spawnPacket << Vec2f{0.f, 0.f};
        spawnPacket << NetworkOwnerComponent::Owner::foreign;
        for(const auto& netId : entityDescriptor.netIds) {
            spawnPacket<< netId;
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

    game.networkEntityOwnershipSystem.removePeer(disconnectedPeer);

    ByteStream dead;
    dead << Packet::DeadEntities;
    for(const auto& entity : disconnectedEntities->second) {
        for(auto netId : entity.netIds) {
            dead << netId;

            EntityId entity = game.online.getEntity(netId);
            if(!entity) continue;

            auto* base = EntitySystem::GetComp<EntityBaseComponent>(entity);
            base->isDead = true;
        }
    }

    game.host.bufferAllDataByChannel(0, dead);
}
