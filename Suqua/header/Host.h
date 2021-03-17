#pragma once
#include "enet/enet.h"
#include "Pool.h"
#include "ByteStream.h"
#include "PacketHandler.h"

#include <string>
#include <deque>
#include <unordered_map>

using PeerId = uint64_t;

//packet with with info about how to send it
//add "packet handlers"
//change sending system to use byte streams

struct DestinedPacket {
	ENetPacket* packet;
	PeerId id;
	size_t channel;
};

class Game;

class Host {
public:
	Host(size_t channelCount_ = 0);
	~Host();
	bool createClient(size_t peerCount, size_t channels = 0, enet_uint32 incomingBandwidth = 0, enet_uint32 outgoingBandwidth = 0);
	bool createServer(int port, size_t peerCount, size_t channels = 0, enet_uint32 incomingBandwidth = 0, enet_uint32 outgoingBandwidth = 0);

	void sendAllData(const ByteStream& data);
	void sendAllDataByChannel(enet_uint8 channel, const ByteStream& data);

	void bufferAllData(const ByteStream& data);
	void bufferAllDataByChannel(enet_uint8 channel, const ByteStream& data);

	void sendData(PeerId id, const ByteStream& data);
	void sendDataByChannel(PeerId id, enet_uint8 channel, const ByteStream& data);

	void bufferData(PeerId id, const ByteStream& data);
	void bufferDataToChannel(PeerId id, enet_uint8 channel, const ByteStream& data);

	void sendBuffered();

	template<typename T, typename ... Args>
	void loadPacketHandler(PacketHandlerId id, Args ... args);

	void handlePackets(Game& game);

	int service(ENetEvent *event, enet_uint32 timeout);
	PeerId tryConnect(const std::string & ip, int port, size_t channels);
	PeerId addPeer(ENetPeer * peer);
	void removePeer(PeerId peerId);
	void disconnect(PeerId peerId);
	void resetConnection(PeerId peerId);
private:
	ENetHost * host;
	size_t channelCount;
	size_t channelIncrementer;
	Pool<ENetPeer *> peers;
	std::deque<PeerId> freeIds;
	std::vector<DestinedPacket> idDestinedPackets;
	std::unordered_map<PacketHandlerId, PacketHandlerPtr> packetHandlers;
	bool connected;
};

template<typename T, typename ...Args>
inline void Host::loadPacketHandler(PacketHandlerId id, Args ...args) {
	auto packetHandler = std::make_unique<T>(id, args...);
	packetHandlers.emplace(id, std::move(packetHandler));
}
