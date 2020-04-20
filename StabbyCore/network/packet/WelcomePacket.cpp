#include "WelcomePacket.h"

void WelcomePacket::serialize() {
	netId = htonll(netId);
	currentTick = htonll(currentTick);
}

void WelcomePacket::unserialize() {
	netId = ntohll(netId);
	currentTick = ntohll(currentTick);
}
