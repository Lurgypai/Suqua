#pragma once
#include <memory>
#include "enet/enet.h"
#include "network.h"

class Connection {
public:
	Connection(ENetPeer & peer_, NetworkId id_, Time_t currentTime);

	Connection(const Connection & other) = delete;
	Connection operator=(const Connection & other) = delete;

	~Connection();

	ENetPeer * getPeer();
	//when the last packet was received
	Time_t getLastPacket() const;
	bool shouldReset() const;

	void setLastPacket(Time_t lastPacket_);
	void setShouldReset(bool reset_);

private:
	ENetPeer * peer;
	//when the last packet was received
	Time_t lastPacket;
	bool reset;
};


using ConnectionPtr = std::unique_ptr<Connection>;