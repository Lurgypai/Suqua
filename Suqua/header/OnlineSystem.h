#pragma once
#include "EntitySystem.h"
#include "NetworkTypes.h"
#include <unordered_map>
#include <deque>

class OnlineSystem {
public:
	OnlineSystem();
	void registerOnlineComponent(EntityId id, NetworkId netId);
	void addOnlineComponent(EntityId id);
	EntityId getEntity(NetworkId netId);

private:
	std::unordered_map<NetworkId, EntityId> ids;
	NetworkId currId;
	std::deque<NetworkId> freeIds;
};