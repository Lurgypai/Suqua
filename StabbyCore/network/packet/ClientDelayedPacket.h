#pragma once
#include "Packet.h"
#define DELAY_KEY "SCD"

//Packet to notify the client their inputs are coming in delayed.
class ClientDelayedPacket {
public:
	ClientDelayedPacket() :
		key{ DELAY_KEY }
	{}

	int8_t key[PACKET_KEY_SIZE + 1];

	// Inherited via Packet
	void serialize();
	void unserialize();
};