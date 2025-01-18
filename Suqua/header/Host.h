#pragma once
#include "enet/enet.h"
#include "ByteStream.h"
#include "PeerId.h"
#include "PacketHandler.h"

#include <chrono>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <OnlineComponent.h>

using ConnectCallback = void(Game& game, PeerId id);
using DisconnectCallback = void(Game& game, PeerId id);

//packet with with info about how to send it
//add "packet handlers"
//change sending system to use byte streams

struct DestinedPacket {
	ENetPacket* packet;
	PeerId id;
	size_t channel;
	bool broadcast;
};

class Game;

class Host {
public:
	enum class Type {
		client = 0,
		server = 1
	};

	Host(size_t channelCount_ = 0);
	~Host();
	void createClient(size_t peerCount, size_t channels = 0, enet_uint32 incomingBandwidth = 0, enet_uint32 outgoingBandwidth = 0);
	void createServer(int port, size_t peerCount, size_t channels = 0, enet_uint32 incomingBandwidth = 0, enet_uint32 outgoingBandwidth = 0);

	void sendAllDataByChannel(enet_uint8 channel, const ByteStream& data);
	void bufferAllDataByChannel(enet_uint8 channel, const ByteStream& data);
	void sendDataByChannel(PeerId id, enet_uint8 channel, const ByteStream& data);
	void bufferDataToChannel(PeerId id, enet_uint8 channel, const ByteStream& data);

	void sendBuffered();

	template<typename T, typename ... Args>
	void loadPacketHandler(PacketId id, Args ... args);

	void handlePackets(Game& game);

	int service(ENetEvent *event, enet_uint32 timeout);
	void tryConnect(const std::string & ip, int port, size_t channels);
	void disconnect(PeerId peerId);
	void resetConnection(PeerId peerId);
	ENetPeer& getPeer(PeerId id);
	PeerId getId(ENetPeer* peer);

	bool isConnected();
	void setConnectCallback(std::function<ConnectCallback> callback);
	void setDisconnectCallback(std::function<DisconnectCallback> callback);

	const std::vector<NetworkId>& getPeerOwnedNetIds(PeerId id);
	bool isPeerConnected(PeerId id);
	size_t getConnectedPeerCount();
    std::vector<PeerId> getConnectedPeers() const;
	size_t getPeerCount();

    void enableDelay(int minDelay, int variation);
    void disableDelay();
    void updateDelayed();
private:
    void broadcastPacket(enet_uint8 channel, ENetPacket* packet);
    void sendPacket(PeerId id, enet_uint8 channel, ENetPacket* packet);

	ENetHost * host;
	size_t channelCount;
	size_t channelIncrementer;
	std::vector<DestinedPacket> idDestinedPackets;
	std::unordered_map<PacketId, PacketHandlerPtr> packetHandlers;
	//if this is a client, a flag for if we're connected to the server
	bool clientConnected;
	std::function<ConnectCallback> connectCallback;
	std::function<DisconnectCallback> disconnectCallback;

	Type type;
    std::vector<bool> connectedPeers;

    bool doDelay;
    struct DelayedPacket {
        bool broadcast;
        PeerId id;
        std::chrono::steady_clock::time_point origin;
        std::chrono::milliseconds delay;
        ENetPacket* packet;
        enet_uint8 channel;
    };
    std::vector<DelayedPacket> delayedPackets;
    int minDelay;
    int delayVariation;
};

//add owning entities

template<typename T, typename ...Args>
inline void Host::loadPacketHandler(PacketId id, Args ...args) {
	auto packetHandler = std::make_unique<T>(id, args...);
	packetHandlers.emplace(id, std::move(packetHandler));
}

//Enet allocates and reuses a pool of peers, so we can just use that :D
//Storage of peers is redundant. Resolve
//ENetPeer* -> PeerId = peer - host->peers;
//PeerId -> ENetPeer& = host->peers[id];
