#include "ServerWorldScene.h"
#include "Game.h"
#include "../Shooty2Core/Shooty2Packet.h"
#include <iostream>
#include "PHServerSpawnEntities.h"
#include "PHServerState.h"
#include "PHServerDeadEntities.h"
#include "NetworkEntityOwnershipSystem.h"
#include "Updater.h"
#include "EntityBaseComponent.h"

#include "../Shooty2Core/AIGunnerComponent.h"
#include "Packet.h"
#include "PHServerAddPlayer.h"
#include "PHServerDamage.h"

#include "PositionComponent.h"

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
    game.loadPacketHandler<PHServerAddPlayer>(Shooty2Packet::AddPlayer);
    game.loadPacketHandler<PHServerDamage>(Shooty2Packet::Damage);
 
    world = World{ "tileset", "levels/test.ldtk" };
	world.load(*this);
    world.getLevel("Level_spawn").activate();

    EntitySpawnSystem::SpawnEntity("enemy.basic", *this, Vec2f{200, 200}, NetworkDataComponent::Owner::local_shared);

    director = Director{};
    director.load(world, *this, "Level_spawn");
}

void ServerWorldScene::physicsStep(Game& game) {
    Updater::UpdateOwned<AIGunnerComponent>(game.PHYSICS_STEP);
	Updater::UpdateOwned<TopDownMoverComponent>();
	Updater::UpdateOwned<ParentComponent>();
	Updater::UpdateOwned<AimToLStickComponent>();
	Updater::UpdateOwned<GunFireComponent>(*this, game.PHYSICS_STEP);
	Updater::UpdateOwned<LifeTimeComponent>();
	Updater::UpdateOwned<HealthWatcherComponent>();
	Updater::UpdateOwned<RespawnComponent>();

	physics.runPhysicsOnOwned(game.PHYSICS_STEP);

    director.update(*this, game.PHYSICS_STEP,
            game.networkEntityOwnershipSystem, game.host);
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
            std::cout << '\t' << entity.tag << '\n';

            spawnPacket << entity.tag;
            auto posComp = EntitySystem::GetComp<PositionComponent>(NetworkDataComponent::GetEntityId(entity.uuid));
            spawnPacket << posComp->pos;
            spawnPacket << entity.uuid;
        }
    }

    for(const auto& entityDescriptor : game.networkEntityOwnershipSystem.getLocalEntities()) {
        std::cout << '\t' << entityDescriptor.tag << '\n';
        spawnPacket<< entityDescriptor.tag;
        auto posComp = EntitySystem::GetComp<PositionComponent>(NetworkDataComponent::GetEntityId(entityDescriptor.uuid));
        spawnPacket << posComp->pos;
        spawnPacket<< entityDescriptor.uuid;
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
    for(const auto& entityDesc : disconnectedEntities->second) {
        dead << entityDesc.uuid;

        EntityId entity = NetworkDataComponent::GetEntityId(entityDesc.uuid);
        std::cout << "Checking UUID " << entityDesc.uuid << " for removal\n";
        if(!entity) continue;

        std::cout << "Setting UUID " << entityDesc.uuid << " to dead.\n";
        auto* base = EntitySystem::GetComp<EntityBaseComponent>(entity);
        base->isDead = true;
    }
    game.host.bufferAllDataByChannel(0, dead);

    game.networkEntityOwnershipSystem.removePeer(disconnectedPeer);
}
