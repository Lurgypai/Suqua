#pragma once
#include "Packet.h"
#define CONT_KEY	"SCT"	//suqua control

//contains state information
class ControllerPacket {
public:
	ControllerPacket() :
		key{ CONT_KEY },
		netId{ 0 },
		clientTime{ 0 },
		state{ 0 }
	{}

	int8_t key[PACKET_KEY_SIZE + 1];

	NetworkId netId;
	//the client side time when the packet was sent
	Time_t clientTime;
	//game time when the packet was sent
	Time_t when;
	enet_uint8 state;
	enet_uint8 prevState;

	bool operator!=(const ControllerPacket & other) {
		return netId != other.netId || clientTime != other.clientTime || when != other.when || state != other.state || prevState != other.prevState;
	}

	// Inherited via Packet
	void serialize();
	void unserialize();
};