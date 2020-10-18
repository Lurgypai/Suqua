#include "SessionSystem.h"
#include <fstream>
#include <cstring>
#include <iostream>
#include <sstream>

SessionSystem::SessionSystem() :
	readOffset{ 0 },
	currTick{0}
{}

void SessionSystem::storeNetworkEvent(NetworkEvent && e, Time_t tick) {
	networkEvents[tick].emplace_back(std::move(e));
	currTick = tick;
}

void SessionSystem::storeControllerEvent(ControllerEvent&& e, Time_t tick) {
	controllerEvents.emplace(tick, std::move(e));
	currTick = tick;
}

const SessionSystem::ControllerEvent& SessionSystem::getControllerEvent() {
	return controllerEvents[currTick];
}

const std::vector<SessionSystem::NetworkEvent>& SessionSystem::getNetworkEvents() {
	return networkEvents[currTick];
}

Time_t SessionSystem::getCurrTick() {
	return currTick;
}

void SessionSystem::tick() {
	++currTick;
}

void SessionSystem::serialize(const std::string& targetFile) {
	std::ofstream out{};
	out.open(targetFile, std::fstream::binary | std::fstream::out);
	if (out.good()) {
		//start at the write tick, and copy any elements into the file in order
		//write the tick
		//write the controller event
		//write amount of enet events
		//write the enet events
		for (Time_t writeTick = 0; writeTick != currTick; writeTick++) {
			if (controllerEvents.find(writeTick) != controllerEvents.end() || networkEvents.find(writeTick) != networkEvents.end()) {
				//allocate
				std::string data;
				size_t offset = 0;
				data.resize(sizeof(writeTick) + sizeof(ControllerEvent) + sizeof(size_t));
				//write the tick
				std::memcpy(data.data(), &writeTick, sizeof(writeTick));
				offset += sizeof(writeTick);
				//write the controller event (if there is one. The first event is the welcome packet)
				if (controllerEvents.find(writeTick) != controllerEvents.end()) {
					std::memcpy(data.data() + offset, &controllerEvents[writeTick], sizeof(ControllerEvent));
					offset += sizeof(ControllerEvent);
				}
				else {
					//for now always write a controller event, so that things are ordered
					ControllerEvent dummy{};
					std::memcpy(data.data() + offset, &dummy, sizeof(ControllerEvent));
					offset += sizeof(ControllerEvent);
				}
				//write how many network events are being stored
				size_t size = networkEvents[writeTick].size();
				std::memcpy(data.data() + offset, &size, sizeof(size_t));
				offset += sizeof(size_t);

				//write the current data
				out << data;

				//write all of the network events
				for (auto& networkEvent : networkEvents[writeTick]) {
					std::string networkEventData{};
					networkEventData.resize(sizeof(NetworkEvent) + sizeof(ENetPacket::dataLength) + networkEvent.enetEvent.packet->dataLength);
					std::memcpy(networkEventData.data(), &networkEvent, sizeof(NetworkEvent));
					std::memcpy(networkEventData.data() + sizeof(NetworkEvent), &networkEvent.enetEvent.packet->dataLength, sizeof(ENetPacket::dataLength));
					std::memcpy(networkEventData.data() + sizeof(NetworkEvent) + sizeof(ENetPacket::dataLength), networkEvent.enetEvent.packet->data, networkEvent.enetEvent.packet->dataLength);
					out << networkEventData;
				}
			}
		}
	}
	else {
		//something went wrong
		throw std::exception{};
	}
}

void SessionSystem::unserialize(const std::string& targetFile) {

	bool firstRead = true;
	std::ifstream in{};
	in.open(targetFile, std::ios::binary | std::ios::in);
	if (in.good()) {
		std::stringstream inStream;
		inStream << in.rdbuf();
		std::string data{inStream.str()};

		unsigned int pos = 0;
		while (pos != data.size()) {
			Time_t writeTick;
			std::memcpy(&writeTick, data.data() + pos, sizeof(writeTick));
			if (firstRead) {
				firstRead = false;
				currTick = writeTick;
			}
			pos += sizeof(writeTick);
			ControllerEvent controllerEvent;
			std::memcpy(&controllerEvent, data.data() + pos, sizeof(ControllerEvent));
			pos += sizeof(ControllerEvent);
			controllerEvents.emplace(writeTick, std::move(controllerEvent));
			size_t size;
			std::memcpy(&size, data.data() + pos, sizeof(size));
			pos += sizeof(size);
			for (auto currNetworkEvent = 0; currNetworkEvent != size; ++currNetworkEvent) {
				NetworkEvent networkEvent;
				std::memcpy(&networkEvent, data.data() + pos, sizeof(NetworkEvent));
				pos += sizeof(NetworkEvent);
				decltype(ENetPacket::dataLength) dataLength;
				std::memcpy(&dataLength, data.data() + pos, sizeof(dataLength));
				pos += sizeof(dataLength);
				networkEvent.enetEvent.packet = enet_packet_create(data.data() + pos, dataLength, 0);
				pos += dataLength;

				networkEvents[writeTick].emplace_back(std::move(networkEvent));
			}
		}
	}
	else {
		throw std::exception{};
	}
}
