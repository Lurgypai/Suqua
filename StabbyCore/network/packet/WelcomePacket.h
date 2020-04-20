#pragma once
#include "Packet.h"
#define WELCOME_KEY "SHI"	//suqua hi

class WelcomePacket  {
public:
	WelcomePacket() :
		key{ WELCOME_KEY },
		currentTick{ 0 },
		netId{ 0 }
	{}

	int8_t key[PACKET_KEY_SIZE + 1];

	NetworkId netId;
	//the serverside time
	Time_t currentTick;

	// Inherited via Packet
	void serialize();
	void unserialize();
};