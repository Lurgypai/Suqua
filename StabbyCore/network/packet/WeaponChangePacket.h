#pragma once
#include "Packet.h"
#define WEAPON_KEY "SWC"

class WeaponChangePacket {
public:
	WeaponChangePacket() :
		key{ WEAPON_KEY },
		size{0}
	{}

	int8_t key[PACKET_KEY_SIZE + 1];
	NetworkId id;

	uint64_t size;

	void serialize();
	void unserialize();
};