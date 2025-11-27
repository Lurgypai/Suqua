#include "NetworkEntityOwnershipSystem.h"
#include <vector>
#include <print>

using OwnedEntityMap = NetworkEntityOwnershipSystem::OwnedEntityMap;
using UUID = Suqua::UUID;

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
    //remove from player set
    players.erase(id);

    //remove from owned entity maps
    for(auto iter = peers.begin(); iter != peers.end(); ++iter) {
        // remove from owned
        for(auto& entity : iter->second) {
            if(entity.uuid != id) continue;
            // store peer before erasing as its invalidated on the next line
            PeerId peer = iter->first;
            peers.erase(iter);

            // remove player if necessary
            auto ownedPlrListIter = ownedPlayers.find(peer);
            if(ownedPlrListIter == ownedPlayers.end()) return;

            // found a list of players associated with that peer
            auto& ownedPlrList = ownedPlrListIter->second;
            for(auto plrIter = ownedPlrList.begin(); plrIter != ownedPlrList.end(); ++plrIter) {
                if(*plrIter != id) continue;
                ownedPlrList.erase(plrIter);
                return; //return as soon as we remove it from the owned players since we're done
            }
            return; // return since we found and removed it
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
    // remove from players set
    for(auto& uuid : ownedPlayers.at(id)) {
        players.erase(uuid);
    }
    // remove from peerid -> player uuid map
    ownedPlayers.erase(id);
    // remove from peerid -> entity uuid map
    peers.erase(id);
}

const OwnedEntityMap& NetworkEntityOwnershipSystem::getOwnedEntities() const {
    return peers;
}

const std::vector<NetworkEntityDescriptor>& NetworkEntityOwnershipSystem::getLocalEntities() const {
    return localEntities;
}

void NetworkEntityOwnershipSystem::addPlayer(PeerId id, UUID uuid) {
    //add to owned player lists
    ownedPlayers[id].push_back(uuid);
    //add to player set
    players.insert(uuid);
}

bool NetworkEntityOwnershipSystem::hasPlayer(PeerId id) const {
    return ownedPlayers.contains(id);
}

const std::unordered_set<Suqua::UUID>& NetworkEntityOwnershipSystem::getPlayers() const {
    return players;
}

const NetworkEntityOwnershipSystem::OwnedPlayerMap& NetworkEntityOwnershipSystem::getOwnedPlayers() const {
    return ownedPlayers;
}
