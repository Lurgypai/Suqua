#include "ControllerPacket.h"

void ControllerPacket::serialize() {
	netId = htonll(netId);
	clientTime = htonll(clientTime);
	when = htonll(when);
}

void ControllerPacket::unserialize() {
	netId = ntohll(netId);
	clientTime = ntohll(clientTime);
	when = ntohll(when);
}
