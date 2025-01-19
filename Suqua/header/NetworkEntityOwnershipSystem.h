#pragma once
#include <vector>
#include <string>
#include <unordered_map>

#include "PeerId.h"
#include "UUID.h"

struct NetworkEntityDescriptor {
    std::string tag;
    std::vector<UUID> uuids;

    NetworkEntityDescriptor(const std::string& tag, std::vector<UUID>&& netIds);
};

/*
 * Broadly, this class tracks which peers (or local) owns what entities
 * This is used for two things currently
 *  When a peer leaves, the entities they owned can be removed.
 *  When a peer joins, we can send them all active entities (including ours)
 */
class NetworkEntityOwnershipSystem {
public:
    using OwnedEntityMap = std::unordered_map<PeerId, std::vector<NetworkEntityDescriptor>>;
    void addOwnedEntity(PeerId peer, const std::string& tag, std::vector<UUID>&& netIds); 
    void addLocalEntity(const std::string& tag, std::vector<UUID>&& netIds);
    // INNEFEFEINT
    void removeEntity(UUID uuid);
    void removeLocalEntity(UUID uuid);
    void removePeer(PeerId id);
    const OwnedEntityMap& getOwnedEntities() const;
    const std::vector<NetworkEntityDescriptor>& getLocalEntities() const;
private:
    OwnedEntityMap peers;
    std::vector<NetworkEntityDescriptor> localEntities;
};
