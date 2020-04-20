#pragma once
#include "enet/enet.h"
#include "../NetworkTypes.h"
#include <string>
#define PACKET_KEY_SIZE 3	//will be plus oned in the packets to hold the null character.

/*
class Packet {
public:
	virtual ~Packet() {};
	virtual void serialize() = 0;
	virtual void unserialize() = 0;
};
*/

namespace PacketUtil {
	inline std::string readPacketKey(ENetPacket * packet) {
		std::string s{};
		s.resize(PACKET_KEY_SIZE);
		std::memcpy(&s[0], packet->data, PACKET_KEY_SIZE);
		return s;
	}

	template<typename T>
	inline void readInto(T& t, ENetPacket * p) {
		std::memcpy(&t, p->data, sizeof(t));
	}

	template<typename T>
	inline void readInto(T * t, ENetPacket * p, size_t count) {
		std::memcpy(t, p->data, sizeof(T) * count);
	}
}