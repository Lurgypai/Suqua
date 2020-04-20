#pragma once
#include "Packet.h"
#define QUIT_KEY	"SQT"	//suqua quit

class QuitPacket {
public:
	QuitPacket() :
		key{ QUIT_KEY },
		id{ 0 }
	{}

	int8_t key[PACKET_KEY_SIZE + 1];

	//the one who quit
	NetworkId id;

	// Inherited via Packet
	void serialize();
	void unserialize();
};