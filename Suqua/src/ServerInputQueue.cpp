#include "ServerInputQueue.h"
#include "ControllerComponent.h"
#include <iostream>

bool ServerInputQueue::allReceived(Host& host, Tick time) {

	if (states.find(time) == states.end())
		return false;

	//for each peer
	auto& map = states.at(time);
	for (PeerId i = 0; i != host.getPeerCount(); ++i) {
		if (host.isPeerConnected(i)) {
			//for each net id owned by that peer
			for (const auto& netId : host.getPeerOwnedNetIds(i)) {
				//if we don't have a state stored, false
				if (map.find(netId) == map.end()) return false;
			}
		}
	}
	return true;
}

bool ServerInputQueue::oneReceived(Host& host, Tick time) {
	if (states.find(time) == states.end())
		return false;

	auto& map = states.at(time);
	return !map.empty();
}

void ServerInputQueue::storeInput(Tick gameTime, NetworkId id, Controller state) {
	states[gameTime][id] = state;
	//std::cout << "Storing input for time " << gameTime << '\n';
}

void ServerInputQueue::applyInputs(const OnlineSystem& online, Tick gameTime) {
	auto pair = states.find(gameTime);
	if (pair != states.end()) {
		auto& map = pair->second;
		for (const auto& [id, cont] : map) {
			auto* comp = EntitySystem::GetComp<ControllerComponent>(online.getEntity(id));
			if (comp) comp->setController(cont);
			else std::cout << "The entity " << id << " has no controller.\n";
		}
		states.erase(states.find(gameTime));
	}
}