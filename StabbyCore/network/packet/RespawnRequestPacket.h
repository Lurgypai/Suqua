#pragma once
#include <cstdint>
#include "Packet.h"
#include "ByteOrder.h"
#define RES_KEY "RES"

class RespawnRequestPacket {
public:
	RespawnRequestPacket() :
		key{ RES_KEY },
		targetRespawnComp{ 0 }
	{}

	uint8_t key[4];
	NetworkId targetRespawnComp;

	void serialize();
	void unserialize();
};