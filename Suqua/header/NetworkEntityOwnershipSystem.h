#pragma once
#include <vector>
#include <string>
#include <unordered_map>

#include "NetworkTypes.h"
#include "PeerId.h"

struct NetworkEntityDescriptor {
    std::string tag;
    std::vector<NetworkId> netIds;

    NetworkEntityDescriptor(const std::string& tag, std::vector<NetworkId>&& netIds);
};

class NetworkEntityOwnershipSystem {
public:
    using OwnedEntityMap = std::unordered_map<PeerId, std::vector<NetworkEntityDescriptor>>;
    void addOwnedEntity(PeerId peer, const std::string& tag, std::vector<NetworkId>&& netIds); 
    void addLocalEntity(const std::string& tag, std::vector<NetworkId>&& netIds);
    // INNEFEFEINT
    void removeEntity(NetworkId netId);
    void removeLocalEntity(NetworkId netId);
    void removePeer(PeerId id);
    const OwnedEntityMap& getOwnedEntities() const;
    const std::vector<NetworkEntityDescriptor>& getLocalEntities() const;
private:
    OwnedEntityMap peers;
    std::vector<NetworkEntityDescriptor> localEntities;
};
