#include "ServerEntityGenerator.h"
#include "../Shooty2Core/Shooty2Packet.h"
#include "Game.h"
#include <cstdint>

ServerEntityGenerator::ServerEntityGenerator(Game* game_) : game{game_} {}

std::vector<EntityId> ServerEntityGenerator::SpawnEntity(const std::string& tag, Scene& targetScene, const Vec2f& targetPos, NetworkDataComponent::Owner owner, const std::vector<UUID>& uuids) {
    auto entities = EntityGenerator::SpawnEntity(tag, targetScene, targetPos, owner, uuids);
    if(owner != NetworkDataComponent::Owner::local_shared) return entities;

    // add to local entities
    // send packet to clients
    ByteStream spawn;
    spawn << Shooty2Packet::SpawnEntities;
    spawn << tag;
    spawn << targetPos;
    spawn << static_cast<std::uint32_t>(entities.size());
    std::vector<UUID> uuidsToSend;
    for(const auto& entityId : entities) {
        auto ndc = EntitySystem::GetComp<NetworkDataComponent>(entityId);
        auto& uuid = ndc->getUUID();
        spawn << uuid;
        uuidsToSend.push_back(uuid);
    }

    // store local entities to be shared with new peers
    game->networkEntityOwnershipSystem.addLocalEntity(tag, std::move(uuidsToSend));

    game->host.bufferAllDataByChannel(0, spawn);
    return entities;
}
