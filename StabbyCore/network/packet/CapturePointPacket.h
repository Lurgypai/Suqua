#pragma once
#include "Packet.h"
#include "../../gamemode/CapturePointComponent.h"
#define CAP_KEY "SCP"

class CapturePointPacket {
public:
	CapturePointPacket() :
		key{ CAP_KEY },
		netId{ 0 }
	{}

	int8_t key[PACKET_KEY_SIZE + 1];

	//online id of the capture point
	NetworkId netId;
	AABB zone;
	CapturePointComponent::StateContainer state;

	//store floats for network transfer
	uint32_t remainingCaptureTime;
	uint32_t totalCaptureTime;
	Vec2i pos;
	Vec2i res;

	// Inherited via Packet
	void serialize();
	void unserialize();
};