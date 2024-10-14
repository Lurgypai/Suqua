#include "NetworkEntityOwnershipSystem.h"

using OwnedEntityMap = NetworkEntityOwnershipSystem::OwnedEntityMap;

NetworkEntityDescriptor::NetworkEntityDescriptor(const std::string& tag_, std::vector<NetworkId>&& netIds_) :
    tag{tag_},
    netIds{netIds_}
{ }

void NetworkEntityOwnershipSystem::addOwnedEntity(PeerId peer, const std::string& tag, std::vector<NetworkId>&& netIds) {
    peers[peer].emplace_back(tag, std::move(netIds));
}

const OwnedEntityMap& NetworkEntityOwnershipSystem::getOwnedEntities() const {
    return peers;
}
