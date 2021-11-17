#pragma once
#include "PacketHandler.h"

namespace Packet {
	//stores a ping packet
	//id
	//source starting gametick
	//server game tick
	constexpr PacketId PingId = 1;
	constexpr PacketId StateId = 2;
	//stores a client input
	//id
	//gametime
	//input state
	constexpr PacketId InputId = 3;
}
