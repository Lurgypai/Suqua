#include "WeaponChangePacket.h"

void WeaponChangePacket::serialize() {
	size = htonll(size);
	id = htonll(id);
}

void WeaponChangePacket::unserialize() {
	size = ntohll(size);
	id = ntohll(id);
}
