#include "QuitPacket.h"
#include "ByteOrder.h"

void QuitPacket::serialize() {
	id = s_htonll(id);
}

void QuitPacket::unserialize() {
	id = s_ntohll(id);
}
