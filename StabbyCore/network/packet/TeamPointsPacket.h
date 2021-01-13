#pragma once
#include <cstdint>
#include "Packet.h"
#define TPP_KEY "TPP"

class TeamPointsPacket {
public:
	TeamPointsPacket() :
		key{ TPP_KEY },
		teamId{ 0 },
		points{0}
	{}

	uint8_t key[4];

	uint32_t teamId;
	uint32_t points;

	void serialize();
	void unserialize();
};