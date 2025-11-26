#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "PeerId.h"
#include "UUID.h"

struct NetworkEntityDescriptor {
    std::string tag;
    Suqua::UUID uuid;

    NetworkEntityDescriptor(const std::string& tag, const Suqua::UUID& uuid);
};

/*
 * Server side only
 * Broadly, this class tracks which peers (or local) owns what entities
 * This is used for two things currently
 *  When a peer leaves, the entities they owned can be removed.
 *  When a peer joins, we can send them all active entities (including ours)
 * In addition, we now track "player" entities so that they can be easily iterated through, and cleanly removed
 */
class NetworkEntityOwnershipSystem {
public:
    using OwnedEntityMap = std::unordered_map<PeerId, std::vector<NetworkEntityDescriptor>>;
    using OwnedPlayerMap = std::unordered_map<PeerId, std::vector<Suqua::UUID>>;
    void addOwnedEntity(PeerId peer, const std::string& tag, const Suqua::UUID& uuid); 
    void addLocalEntity(const std::string& tag, const Suqua::UUID& uuid);
    // INNEFICIENT
    void removeEntity(Suqua::UUID uuid);
    void removeLocalEntity(Suqua::UUID uuid);
    void removePeer(PeerId id);
    const OwnedEntityMap& getOwnedEntities() const;
    const std::vector<NetworkEntityDescriptor>& getLocalEntities() const;

    // add a player owned by the specified client
    void addPlayer(PeerId id, Suqua::UUID uuid);
    // checks if the specified peer has a player
    bool hasPlayer(PeerId id) const;
    const std::unordered_set<Suqua::UUID>& getPlayers() const;
    const OwnedPlayerMap& getOwnedPlayers() const;
private:
    // What peers own what entities
    OwnedEntityMap peers;

    std::vector<NetworkEntityDescriptor> localEntities;

    // what peers own what player entities, repeates the owned entity map entities
    OwnedPlayerMap ownedPlayers;

    // list of all player entities
    std::unordered_set<Suqua::UUID> players;
};
