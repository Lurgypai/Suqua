#include "WelcomePacket.h"
#include "ByteOrder.h"

void WelcomePacket::serialize() {
	netId = s_htonll(netId);
	currentTick = s_htonll(currentTick);
	teamCount = s_htonl(teamCount);
	capPoints = s_htonl(capPoints);
	winningPoints = s_htonl(winningPoints);
}

void WelcomePacket::unserialize() {
	netId = s_ntohll(netId);
	currentTick = s_ntohll(currentTick);
	teamCount = s_ntohl(teamCount);
	capPoints = s_ntohl(capPoints);
	winningPoints = s_ntohl(winningPoints);
}
