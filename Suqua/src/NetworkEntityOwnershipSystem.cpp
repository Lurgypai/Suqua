#include "NetworkEntityOwnershipSystem.h"
#include <vector>

using OwnedEntityMap = NetworkEntityOwnershipSystem::OwnedEntityMap;

NetworkEntityDescriptor::NetworkEntityDescriptor(const std::string& tag_,
        std::vector<UUID>&& uuids_) :
    tag{tag_},
    uuids{uuids_}
{}

void NetworkEntityOwnershipSystem::addOwnedEntity(PeerId peer, const std::string& tag, std::vector<UUID>&& uuids) {
    peers[peer].emplace_back(tag, std::move(uuids));
}

void NetworkEntityOwnershipSystem::addLocalEntity(const std::string& tag, std::vector<UUID>&& uuids) {
    localEntities.emplace_back(tag, std::move(uuids));
}

void NetworkEntityOwnershipSystem::removeEntity(UUID id) {
    for(auto iter = peers.begin(); iter != peers.end(); ++iter) {
        for(auto& entity : iter->second) {
            for(const auto& uuid : entity.uuids) {
                if(uuid != id) continue;
                peers.erase(iter);
                return;
            }
        }
    }
}

void NetworkEntityOwnershipSystem::removeLocalEntity(UUID id) {
    for(auto iter = localEntities.begin(); iter != localEntities.end(); ++iter) {
        for(const auto& uuid : iter->uuids) {
            if(uuid != id) continue;
            localEntities.erase(iter);
            return;
        }
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
