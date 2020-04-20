#pragma once
#include "Packet.h"

#define TIME_KEY	"STS"	//suqua time stamp

class TimestampPacket {
public:
	TimestampPacket() :
		key{ TIME_KEY },
		id{ 0 },
		clientTime{ 0 },
		serverTime{ 0 },
		gameTime{ 0 }
	{}

	int8_t key[PACKET_KEY_SIZE + 1];

	NetworkId id;
	//when the packet was sent from the server (in local client ticks)
	Time_t clientTime;
	//what time the packet was recieved on the server (in local server ticks)
	Time_t serverTime;
	//the game time according to the server
	Time_t gameTime;

	// Inherited via Packet
	void serialize();
	void unserialize();
};