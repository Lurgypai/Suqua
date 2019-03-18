#include "pch.h"
#include "Connection.h"

Connection::Connection(ENetPeer & peer_, NetworkId id_, Time_t currentTime) :
	peer{ &peer_ },
	lastPacket{ currentTime },
	reset{ false }
{
	NetworkId * id_ptr = new NetworkId(id_);
	peer->data = id_ptr;
}

Connection::~Connection() {
	if (peer->data) {
		delete peer->data;
	}
}

ENetPeer * Connection::getPeer() {
	return peer;
}

Time_t Connection::getLastPacket() const {
	return lastPacket;
}

bool Connection::shouldReset() const {
	return reset;
}

void Connection::setLastPacket(Time_t lastPacket_) {
	lastPacket = lastPacket_;
}

void Connection::setShouldReset(bool reset_) {
	reset = reset_;
}
