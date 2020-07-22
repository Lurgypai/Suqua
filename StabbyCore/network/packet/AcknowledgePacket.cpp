#include "AcknowledgePacket.h"
#include "ByteOrder.h"

void AcknowledgePacket::serialize() {
	netId = s_htonll(netId);
	stateId = s_htonll(stateId);
}

void AcknowledgePacket::unserialize() {
	netId = s_ntohll(netId);
	stateId = s_ntohll(stateId);
}