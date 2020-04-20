#pragma once
#include "Packet.h"
#define MESSAGE_KEY	"SMG"	//suqua quit

class MessagePacket {
public:
	MessagePacket() :
		key{ MESSAGE_KEY }
	{}

	int8_t key[PACKET_KEY_SIZE + 1];

	char message[256];

	// Inherited via Packet
	void serialize();
	void unserialize();
};