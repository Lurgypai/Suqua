#include "NetworkEntityOwnershipSystem.h"
#include <vector>

using OwnedEntityMap = NetworkEntityOwnershipSystem::OwnedEntityMap;

NetworkEntityDescriptor::NetworkEntityDescriptor(const std::string& tag_,
        const UUID& uuid_) :
    tag{tag_},
    uuid{uuid_}
{}

void NetworkEntityOwnershipSystem::addOwnedEntity(PeerId peer, const std::string& tag, const UUID& uuid) {
    peers[peer].emplace_back(tag, uuid);
}

void NetworkEntityOwnershipSystem::addLocalEntity(const std::string& tag, const UUID& uuid) {
    localEntities.emplace_back(tag, uuid);
}

void NetworkEntityOwnershipSystem::removeEntity(UUID id) {
    for(auto iter = peers.begin(); iter != peers.end(); ++iter) {
        for(auto& entity : iter->second) {
            if(entity.uuid != id) continue;
            peers.erase(iter);
            return;
        }
    }
}

void NetworkEntityOwnershipSystem::removeLocalEntity(UUID id) {
    for(auto iter = localEntities.begin(); iter != localEntities.end(); ++iter) {
        if(iter->uuid != id) continue;
        localEntities.erase(iter);
        return;
    }
}

void NetworkEntityOwnershipSystem::removePeer(PeerId id) {
    peers.erase(id);
}

const OwnedEntityMap& NetworkEntityOwnershipSystem::getOwnedEntities() const {
    return peers;
}

const std::vector<NetworkEntityDescriptor>& NetworkEntityOwnershipSystem::getLocalEntities() const {
    return localEntities;
}
