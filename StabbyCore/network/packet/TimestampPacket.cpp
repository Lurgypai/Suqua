#include "TimestampPacket.h"
#include "ByteOrder.h"

void TimestampPacket::serialize() {
	id = s_htonll(id);
	clientTime = s_htonll(clientTime);
	serverTime = s_htonll(serverTime);
	gameTime = s_htonll(gameTime);
}

void TimestampPacket::unserialize() {
	id = s_ntohll(id);
	clientTime = s_ntohll(clientTime);
	serverTime = s_ntohll(serverTime);
	gameTime = s_ntohll(gameTime);
}
