#pragma once
#include <cstdint>
#include "Packet.h"
#include "../../gamestate/GameState.h"
#define ACK_KEY "ACK"

class AcknowledgePacket {
public:
	AcknowledgePacket() :
		key{ ACK_KEY },
		netId{ 0 },
		stateId{0}
	{}

	uint8_t key[4];

	NetworkId netId;
	GameStateId stateId;

	void serialize();
	void unserialize();
};