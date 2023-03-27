#pragma once
#include "OnlineSystem.h"
#include "Tick.h"
#include "Controller.h"
#include "Host.h"
#include <unordered_map>

class ServerInputQueue {
public:
	bool allReceived(Host& host, Tick time);
	// received input from at least one connected peer
	bool oneReceived(Host& host, Tick time);
	void storeInput(Tick gameTime, NetworkId id, Controller state);
	void applyInputs(const OnlineSystem& online, Tick gameTime);
private:
	std::unordered_map<NetworkId, std::unordered_map<Tick, Controller>> states;
};