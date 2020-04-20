#pragma once
#include "Packet.h"
#define JOIN_KEY	"SJN"	//suqua join

//sent to all previously connected clients when a new player joins
class JoinPacket {
public:
	JoinPacket() :
		key{ JOIN_KEY },
		joinerId{ 0 }
	{}

	int8_t key[PACKET_KEY_SIZE + 1];

	//id of the new person
	NetworkId joinerId;

	// Inherited via Packet
	void serialize();
	void unserialize();
};