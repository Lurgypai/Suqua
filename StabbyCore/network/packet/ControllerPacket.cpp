#include "ControllerPacket.h"
#include "ByteOrder.h"

void ControllerPacket::serialize() {
	netId = s_htonll(netId);
	clientTime = s_htonll(clientTime);
	when = s_htonll(when);
}

void ControllerPacket::unserialize() {
	netId = s_ntohll(netId);
	clientTime = s_ntohll(clientTime);
	when = s_ntohll(when);
}
