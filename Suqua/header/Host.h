#pragma once
#include "enet/enet.h"
#include "Pool.h"
#include "ByteStream.h"
#include "PeerId.h"
#include "PacketHandler.h"
#include "DynamicBitset.h"

#include <string>
#include <deque>
#include <unordered_map>
#include <functional>
#include <vector>
#include <OnlineComponent.h>

using ConnectCallback = void(const ENetEvent&);
using DisconnectCallback = void(Game& game, const ENetEvent&);

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

    //send to data to all peers
	void sendAllData(const ByteStream& data);
    //send to data to all peers using specified channel
	void sendAllDataByChannel(enet_uint8 channel, const ByteStream& data);

    //buffer to data to all peers
	void bufferAllData(const ByteStream& data);
    //buffer data to all peers using specified channel;
	void bufferAllDataByChannel(enet_uint8 channel, const ByteStream& data);

	void sendData(PeerId id, const ByteStream& data);
	void sendDataByChannel(PeerId id, enet_uint8 channel, const ByteStream& data);

	void bufferData(PeerId id, const ByteStream& data);
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

	void addNetIdToPeer(PeerId peerId, NetworkId netId);
	void removeNetIdFromPeer(PeerId peerId, NetworkId netId);

	const std::vector<NetworkId>& getPeerOwnedNetIds(PeerId id);
	bool isPeerConnected(PeerId id);
	size_t getConnectedPeerCount();
	size_t getPeerCount();
private:
	ENetHost * host;
	size_t channelCount;
	size_t channelIncrementer;
	std::vector<DestinedPacket> idDestinedPackets;
	std::unordered_map<PacketId, PacketHandlerPtr> packetHandlers;
	//if this is a client, a flag for if we're connected to the server
	bool clientConnected;
	std::function<ConnectCallback> connectCallback;
	std::function<DisconnectCallback> disconnectCallback;
	std::vector<std::vector<NetworkId>> ownedNetIds;

	Type type;
	DynamicBitset connectedPeers;
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