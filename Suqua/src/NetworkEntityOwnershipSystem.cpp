#include "NetworkEntityOwnershipSystem.h"
#include <vector>

using OwnedEntityMap = NetworkEntityOwnershipSystem::OwnedEntityMap;

NetworkEntityDescriptor::NetworkEntityDescriptor(const std::string& tag_, std::vector<NetworkId>&& netIds_) :
    tag{tag_},
    netIds{netIds_}
{}

void NetworkEntityOwnershipSystem::addOwnedEntity(PeerId peer, const std::string& tag, std::vector<NetworkId>&& netIds) {
    peers[peer].emplace_back(tag, std::move(netIds));
}

void NetworkEntityOwnershipSystem::addLocalEntity(const std::string& tag, std::vector<NetworkId>&& netIds) {
    localEntities.emplace_back(tag, std::move(netIds));
}

void NetworkEntityOwnershipSystem::removeEntity(NetworkId id) {
    for(auto iter = peers.begin(); iter != peers.end(); ++iter) {
        for(auto& entity : iter->second) {
            for(const auto& netId : entity.netIds) {
                if(netId != id) continue;
                peers.erase(iter);
                return;
            }
        }
    }
}

void NetworkEntityOwnershipSystem::removeLocalEntity(NetworkId id) {
    for(auto iter = localEntities.begin(); iter != localEntities.end(); ++iter) {
        for(const auto& netId : iter->netIds) {
            if(netId != id) continue;
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
