#include "TeamChangePacket.h"
#include "ByteOrder.h"


void TeamChangePacket::serialize() {
	targetTeamId = s_htonll(targetTeamId);
	id = s_htonll(id);
}

void TeamChangePacket::unserialize() {
	targetTeamId = s_ntohll(targetTeamId);
	id = s_ntohll(id);
}
