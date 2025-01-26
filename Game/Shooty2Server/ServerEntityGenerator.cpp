#include "ServerEntityGenerator.h"
#include "../Shooty2Core/Shooty2Packet.h"
#include "Game.h"

ServerEntityGenerator::ServerEntityGenerator(Game* game_) : game{game_} {}

EntityId ServerEntityGenerator::SpawnEntity(
        const std::string& tag,
        Scene& targetScene,
        const Vec2f& targetPos,
        NetworkDataComponent::Owner owner,
        const UUID& uuid) {
    auto entity = EntityGenerator::SpawnEntity(tag, targetScene, targetPos, owner, uuid);
    if(owner != NetworkDataComponent::Owner::local_shared) return entity;

    // add to local entities
    // send packet to clients
    ByteStream spawn;
    spawn << Shooty2Packet::SpawnEntities;
    spawn << tag;
    spawn << targetPos;
    spawn << uuid;

    // store local entities to be shared with new peers
    game->networkEntityOwnershipSystem.addLocalEntity(tag, uuid);

    game->host.bufferAllDataByChannel(0, spawn);
    return entity;
}
