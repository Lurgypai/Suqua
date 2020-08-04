#include "NameTagPacket.h"
#include "ByteOrder.h"

void NameTagPacket::serialize() {
	netId = s_htonll(netId);
}

void NameTagPacket::unserialize() {
	netId = s_ntohll(netId);
}