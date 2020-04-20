#include "JoinPacket.h"

void JoinPacket::serialize() {
	joinerId = htonll(joinerId);
}

void JoinPacket::unserialize() {
	joinerId = ntohll(joinerId);
}
