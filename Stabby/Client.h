#pragma once
#include <string>
#include "enet/enet.h"
#include "Packet.h"
#include "EntitySystem.h"
#include "Stage.h"
#include <deque>

#define PING_COUNT 10

class Client {
public:
	Client(const Stage& stage_);

	~Client();

	void connect(Time_t now, const std::string & ip, int port);

	void send(ENetPacket * p);

	void service(Time_t now_);

	void ping(Time_t now);
	void recalculatePing(Time_t nextPing);

	//returns ping in milliseconds
	unsigned int getPing();

	bool getConnected();

	Time_t getTime();
	void progressTime(Time_t delta);

	NetworkId getNetId();

	void setPlayer(EntityId id_);
private:
	void receive(ENetEvent & e);

	
	//client time
	Time_t now;
	//the last PING_COUT pings.
	std::deque<Time_t> pings;
	//average ping in local client time
	Time_t currentPing;

	ENetHost * client;
	ENetPeer * peer;
	bool connected;
	NetworkId id;
	EntityId playerId;
	//gameTime
	Time_t networkTime;
	const Stage& stage;
};