#pragma once
#include <vector>
#include <string>
#include <unordered_map>

#include "PeerId.h"
#include "UUID.h"

struct NetworkEntityDescriptor {
    std::string tag;
    Suqua::UUID uuid;

    NetworkEntityDescriptor(const std::string& tag, const Suqua::UUID& uuid);
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
    void addOwnedEntity(PeerId peer, const std::string& tag, const Suqua::UUID& uuid); 
    void addLocalEntity(const std::string& tag, const Suqua::UUID& uuid);
    // INNEFEFEINT
    void removeEntity(Suqua::UUID uuid);
    void removeLocalEntity(Suqua::UUID uuid);
    void removePeer(PeerId id);
    const OwnedEntityMap& getOwnedEntities() const;
    const std::vector<NetworkEntityDescriptor>& getLocalEntities() const;
private:
    OwnedEntityMap peers;
    std::vector<NetworkEntityDescriptor> localEntities;
};
