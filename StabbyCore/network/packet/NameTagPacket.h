#pragma once
#include "Packet.h"
#include "CharBuffer.h"
#include "../../player/NameTagComponent.h"
#define NAMETAG_KEY "NTP"


class NameTagPacket {
public:
	NameTagPacket() :
		key{NAMETAG_KEY},
		netId{0},
		nameTag{}
	{}

	unsigned char key[4];
	NetworkId netId;
	CharBuffer<NameTagComponent::SIZE> nameTag;

	void serialize();
	void unserialize();
};