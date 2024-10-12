#include "OnlineSystem.h"
#include "OnlineComponent.h"

OnlineSystem::OnlineSystem() :
	ids{},
	currId{0},
	freeIds{}
{}

NetworkId OnlineSystem::getFreeNetworkId() {
	NetworkId netId = currId;
	if (!freeIds.empty()) {
		netId = freeIds.back();
		freeIds.pop_back();
	}
	else {
		++currId;
	}
    return netId;
}

void OnlineSystem::registerOnlineComponent(EntityId id, NetworkId netId) {

	if (netId >= currId) {
		unsigned int idCount = netId - currId;
		for (unsigned int i = 0; i != idCount; ++i) {
			freeIds.push_front(currId + i);
		}
		currId = netId + 1;
	}
	else {
		bool idIsFree = false;
		for (auto& id : freeIds) {
			if (id == netId) {
				idIsFree = true;
				break;
			}
		}
		if (!idIsFree) {
			//throw an error, unable to get id
			throw std::exception{};
		}
	}

	if(!EntitySystem::Contains<OnlineComponent>() || EntitySystem::GetComp<OnlineComponent>(id) == nullptr)
		EntitySystem::MakeComps<OnlineComponent>(1, &id);

	EntitySystem::GetComp<OnlineComponent>(id)->netId = netId;
	ids[netId] = id;
}

void OnlineSystem::addOnlineComponent(EntityId id) {
	if (!EntitySystem::Contains<OnlineComponent>() || EntitySystem::GetComp<OnlineComponent>(id) == nullptr)
		EntitySystem::MakeComps<OnlineComponent>(1, &id);

	NetworkId netId = getFreeNetworkId();

	EntitySystem::GetComp<OnlineComponent>(id)->netId = netId;
	ids[netId] = id;
}

EntityId OnlineSystem::getEntity(NetworkId netId) const{
	auto id = ids.find(netId);
	if (id != ids.end())
		return id->second;
	else
		return 0;
}
