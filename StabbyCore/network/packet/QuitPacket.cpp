#include "QuitPacket.h"

void QuitPacket::serialize() {
	id = htonll(id);
}

void QuitPacket::unserialize() {
	id = ntohll(id);
}
