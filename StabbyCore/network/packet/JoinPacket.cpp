#include "JoinPacket.h"
#include "ByteOrder.h"

void JoinPacket::serialize() {
	joinerId = s_htonll(joinerId);
}

void JoinPacket::unserialize() {
	joinerId = s_ntohll(joinerId);
}
