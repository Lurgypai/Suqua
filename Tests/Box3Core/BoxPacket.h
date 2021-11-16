#pragma once
#include "PacketHandler.h"

namespace BoxPacket {
	//notify a client that it can create its own packet
	//packet id, network id
	constexpr PacketId ClientNetworkIdPacket = 10;
	//notify a client that a different client has joined
	//packet id, network id
	constexpr PacketId JoinPacket = 11;
	//notify a client that a different client has left
	//packet id, network id
	constexpr PacketId LeavePacket = 12;
};