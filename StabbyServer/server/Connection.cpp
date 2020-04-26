
#include "Connection.h"

Connection::Connection(ENetPeer & peer_, PeerId id_, Time_t currentTime) :
	peer{ &peer_ },
	lastPacket{ currentTime },
	reset{ false }
{
	PeerId * id_ptr = new PeerId(id_);
	peer->data = id_ptr;
}

Connection::~Connection() {
	if (peer->data) {
		delete static_cast<PeerId*>(peer->data);
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
