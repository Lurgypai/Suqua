#pragma once
#include <string>
#include <unordered_map>
#include "network.h"

class SessionSystem {
public:
	struct ControllerEvent {
		unsigned char controllerState;
	};

	struct NetworkEvent {
		ENetEvent enetEvent;
	};

	SessionSystem();
	void storeNetworkEvent(NetworkEvent&& e, Time_t tick);
	void storeControllerEvent(ControllerEvent&& e, Time_t tick);
	const ControllerEvent& getControllerEvent();
	const std::vector<NetworkEvent>& getNetworkEvents();
	Time_t getCurrTick();
	void tick();

	void serialize(const std::string& targetFile);
	void unserialize(const std::string& targetFile);
private:
	//the session will now be in charge of what tick of the game it is.
	Time_t currTick;
	std::unordered_map<Time_t, std::vector<NetworkEvent>> networkEvents;
	std::unordered_map<Time_t, ControllerEvent> controllerEvents;
	size_t readOffset;
};

//separate controller and enet events
//make them accessible by frame