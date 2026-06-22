#include "ServerWorldScene.h"

#include <print>

#include "Game.h"

#include "NetworkEntityOwnershipSystem.h"
#include "Packet.h"
#include "PHServerSpawnEntities.h"
#include "PHServerState.h"
#include "PHServerDeadEntities.h"
#include "PHServerAddPlayer.h"
#include "PHServerDamage.h"

#include "ServerEntityGenerator.h"
#include "Updater.h"

#include "EntityBaseComponent.h"
#include "PositionComponent.h"
#include "TopDownMoverComponent.h"
#include "AimToLStickComponent.h"
#include "LifeTimeComponent.h"

#include "../Shooty2Core/RespawnComponent.h"
#include "../Shooty2Core/Shooty2Packet.h"
#include "../Shooty2Core/AIGunnerComponent.h"
#include "../Shooty2Core/HandComponent.h"

ServerWorldScene::ServerWorldScene(SceneId id_, Scene::FlagType flags_,
        ItemSystem& items_, const std::string& worldFile) :
	Scene{id_, flags_},
    physics{},
    items{items_},
    world{worldFile, *this, physics},
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
 

    director = Director{};
    director.load(world, *this, "Level_spawn");
    world.getLevel("Level_spawn").activate();
}

void ServerWorldScene::physicsStep(Game& game) {
    Updater::UpdateOwned<AIGunnerComponent>(game.PHYSICS_STEP, items);
	Updater::UpdateOwned<TopDownMoverComponent>();
	Updater::UpdateOwned<AimToLStickComponent>();
	Updater::UpdateOwned<LifeTimeComponent>();
	Updater::UpdateOwned<RespawnComponent>();
	Updater::UpdateOwned<HandComponent>(*this, game.PHYSICS_STEP);

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
    std::println("Peer {} connection received."
            " Sending existing entities.", connectingPeer);
    ByteStream spawnPacket;
    spawnPacket << Shooty2Packet::SpawnEntities;
    for(const auto& [peer, entities] : game.networkEntityOwnershipSystem.getOwnedEntities()) {
        if(peer == connectingPeer) continue;

        for(const auto& entity : entities) {
            spawnPacket << entity.tag;
            auto posComp = EntitySystem::GetComp<PositionComponent>(
                    NetworkDataComponent::GetEntityId(entity.uuid));
            spawnPacket << posComp->pos;
            spawnPacket << entity.uuid;
            std::println("\t {}: {}, {}", entity.tag, posComp->pos.x, posComp->pos.y);
        }
    }

    for(const auto& entityDescriptor : game.networkEntityOwnershipSystem.getLocalEntities()) {
        spawnPacket<< entityDescriptor.tag;
        auto posComp = EntitySystem::GetComp<PositionComponent>(
                NetworkDataComponent::GetEntityId(entityDescriptor.uuid));
        spawnPacket << posComp->pos;
        spawnPacket<< entityDescriptor.uuid;
        std::println("\t {}: {}, {}", entityDescriptor.tag, posComp->pos.x, posComp->pos.y);
    }

    game.host.bufferDataToChannel(connectingPeer, 0, spawnPacket);
}

void ServerWorldScene::onDisconnect(Game& game, PeerId disconnectedPeer) {
    std::println("peer {} disconnected.", disconnectedPeer);
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
