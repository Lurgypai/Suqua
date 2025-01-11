#include "ServerEntityGenerator.h"
#include "../Shooty2Core/Shooty2Packet.h"
#include "OnlineComponent.h"
#include "Game.h"

ServerEntityGenerator::ServerEntityGenerator(Game* game_) : game{game_} {}

std::vector<EntityId> ServerEntityGenerator::SpawnEntity(const std::string& tag, Scene& targetScene, const Vec2f& targetPos, NetworkOwnerComponent::Owner owner, bool shared) {
    auto entities = EntityGenerator::SpawnEntity(tag, targetScene, targetPos, owner, shared);
    if(!shared) return entities;

    // add to local entities
    // send packet to clients
    ByteStream spawn;
    spawn << Shooty2Packet::SpawnEntities;
    spawn << tag;
    spawn << targetPos;
    spawn << NetworkOwnerComponent::Owner::foreign;
    std::vector<NetworkId> netIds{};
    for(const auto& entityId : entities) {
        game->online.addOnlineComponent(entityId);
        OnlineComponent* online = EntitySystem::GetComp<OnlineComponent>(entityId);
        spawn << online->getNetId();
        netIds.push_back(online->getNetId());
    }

    game->networkEntityOwnershipSystem.addLocalEntity(tag, std::move(netIds));

    game->host.bufferAllDataByChannel(0, spawn);
    return entities;
}
