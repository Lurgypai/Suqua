#include "RespawnRequestPacket.h"

void RespawnRequestPacket::serialize() {
	targetRespawnComp = s_htonll(targetRespawnComp);
}

void RespawnRequestPacket::unserialize() {
	targetRespawnComp = s_ntohll(targetRespawnComp);
}
