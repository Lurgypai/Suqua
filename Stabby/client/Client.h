#pragma once
#include <string>
#include "enet/enet.h"
#include "EntitySystem.h"
#include "stage.h"
#include <deque>

#include "network.h"
#include "combat.h"
#include "gamemode.h"

#include "../player/ClientPlayerSystem.h"

#define PING_COUNT 10

class Client {
public:
	Client();

	~Client();

	void connect(const std::string & ip, int port);

	template<typename Packet>
	void send(Packet p);

	void send(size_t size, void* data);

	void service();

	void ping();
	void recalculatePing(Time_t nextPing);

	//returns ping in milliseconds
	unsigned int getPing();

	bool getConnected();

	//the GameTime synchronized across all clients and the server
	Time_t getTime();
	void progressTime(Time_t delta);

	void setPlayer(EntityId id_);
	bool isBehindServer();
	//once we've told the server the time, and what we're doing, this is done to reset that we are behind.
	void resetBehindServer();

	void setWeaponManager(WeaponManager& weapons_);
	void setClientPlayerSystem(ClientPlayerSystem* clientPlayer);
	void setOnlineSystem(OnlineSystem* online);
	void setMode(DominationMode* mode);
	void setSpawns(SpawnSystem* spawns);

	const std::vector<NetworkId>& getNewPlayers() const;
	void clearNewPlayers();
	//client time
	Time_t clientTime;
private:
	void receive(ENetEvent & e);
	//the last PING_COUT pings.
	std::deque<Time_t> pings;
	//average ping in local client time
	Time_t currentPing;

	PeerId serverId;
	EntityId playerId;
	bool connected;
	//gameTime
	Time_t networkTime;
	//if we're behind the server, so that we can tell the server the current time.
	bool behindServer;
	Pool<EntityId> idTable;
	Host client;
	WeaponManager* weapons;
	ClientPlayerSystem* clientPlayers;
	OnlineSystem* online;
	DominationMode* mode;
	SpawnSystem* spawns;

	std::vector<NetworkId> toJoinIds;
};

template<typename Packet>
inline void Client::send(Packet p) {
	client.sendPacket<Packet>(serverId, 0, p);
}
