#include "WeaponChangePacket.h"
#include "ByteOrder.h"

void WeaponChangePacket::serialize() {
	size = s_htonll(size);
	id = s_htonll(id);
}

void WeaponChangePacket::unserialize() {
	size = s_ntohll(size);
	id = s_ntohll(id);
}
