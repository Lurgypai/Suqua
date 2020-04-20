#include "TeamChangePacket.h"


void TeamChangePacket::serialize() {
	targetTeamId = htonll(targetTeamId);
	id = htonll(id);
}

void TeamChangePacket::unserialize() {
	targetTeamId = ntohll(targetTeamId);
	id = ntohll(id);
}
