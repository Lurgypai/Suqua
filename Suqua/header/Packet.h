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
	//notifies a client that it is out of sync, and needs to send a ping
	//id
	constexpr PacketId OOSId = 4;

    constexpr PacketId DeadEntities = 5;
    /*
     * DeadEntities:
     * Client->server / server->client
     *  NetworkId netId
     */
}
