#pragma once
#include "Packet.h"
#define TEAM_KEY "STC"

class TeamChangePacket {
public:
	TeamChangePacket() :
		key{ TEAM_KEY },
		targetTeamId{ 0 }
	{}

	int8_t key[PACKET_KEY_SIZE + 1];
	NetworkId id;

	uint64_t targetTeamId;

	void serialize();
	void unserialize();
};