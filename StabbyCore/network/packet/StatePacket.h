#pragma once
#include "Packet.h"
#include "MarkedStream.h"
#include "DynamicBitset.h"
#include "../../player/PlayerStateComponent.h"
#define STATE_KEY	"SST"	//suqua state

class StatePacket {
public:
	StatePacket() :
		key{ STATE_KEY },
		state{},
		id{ 0 }
	{}

	int8_t key[PACKET_KEY_SIZE + 1];

	//the player who moved
	NetworkId id;
	//game time when the packet was sent
	Time_t when;
	PlayerState state;

	//these are used for storing floats/doubles over the network
	Vec2i pos;
	Vec2i vel;
	Vec2i spawnPoint;
	uint64_t attackSpeed;
	uint64_t moveSpeed;

	//used for client side graphical prediction
	unsigned char controllerState;
	unsigned char prevControllerState;

	// Inherited via Packet
	void serialize();
	void unserialize();
	void readInto(MarkedStream& m, const PlayerState& prevState);
	void readInto(MarkedStream& m);
	//returns an in order list of what was modified
	DynamicBitset readFrom(MarkedStream& m);
};