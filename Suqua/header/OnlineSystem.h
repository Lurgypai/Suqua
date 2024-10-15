#pragma once
#include "EntitySystem.h"
#include "NetworkTypes.h"
#include <unordered_map>
#include <deque>

class OnlineSystem {
public:
	OnlineSystem();
    NetworkId getFreeNetworkId();
	void registerOnlineComponent(EntityId id, NetworkId netId);
	void addOnlineComponent(EntityId id);
	EntityId getEntity(NetworkId netId) const;
    void freeNetId(NetworkId id);

private:
	std::unordered_map<NetworkId, EntityId> ids;
	NetworkId currId;
	std::deque<NetworkId> freeIds;
};
