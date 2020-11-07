#pragma once
#include "Packet.h"
#include "CharBuffer.h"
#define COMMAND_KEY "SCD"

//Packet to notify the client their inputs are coming in delayed.
class CommandPacket {
public:
	CommandPacket() :
		key{ COMMAND_KEY }
	{}

	int8_t key[PACKET_KEY_SIZE + 1];
	CharBuffer<256> command;

	// Inherited via Packet
	void serialize();
	void unserialize();
};