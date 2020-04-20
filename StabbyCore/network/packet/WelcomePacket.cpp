#include "WelcomePacket.h"
#include "ByteOrder.h"

void WelcomePacket::serialize() {
	netId = s_htonll(netId);
	currentTick = s_htonll(currentTick);
}

void WelcomePacket::unserialize() {
	netId = s_ntohll(netId);
	currentTick = s_ntohll(currentTick);
}
