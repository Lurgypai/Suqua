#include "TimestampPacket.h"

void TimestampPacket::serialize() {
	id = htonll(id);
	clientTime = htonll(clientTime);
	serverTime = htonll(serverTime);
	gameTime = htonll(gameTime);
}

void TimestampPacket::unserialize() {
	id = ntohll(id);
	clientTime = ntohll(clientTime);
	serverTime = ntohll(serverTime);
	gameTime = ntohll(gameTime);
}
