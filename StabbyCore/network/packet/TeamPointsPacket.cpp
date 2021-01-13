#include "TeamPointsPacket.h"
#include "ByteOrder.h"

void TeamPointsPacket::serialize() {
	teamId = s_htonl(teamId);
	points = s_htonl(points);
}

void TeamPointsPacket::unserialize() {
	teamId = s_ntohl(teamId);
	points = s_ntohl(points);
}