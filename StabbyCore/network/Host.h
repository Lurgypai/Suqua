#pragma once
#include "enet/enet.h"
#include "Pool.h"

#include <string>
#include <deque>

using PeerId = uint64_t;

//packet with with info about how to send it
struct DestinedPacket {
	ENetPacket* packet;
	PeerId id;
	size_t channel;
};

class Host {
public:
	Host(size_t channelCount_ = 0);
	~Host();
	bool createClient(size_t peerCount, size_t channels = 0, enet_uint32 incomingBandwidth = 0, enet_uint32 outgoingBandwidth = 0);
	bool createServer(int port, size_t peerCount, size_t channels = 0, enet_uint32 incomingBandwidth = 0, enet_uint32 outgoingBandwidth = 0);

	template<typename Packet>
	void sendPacket(PeerId id, Packet packet);
	template<typename Packet>
	void sendPacketByChannel(PeerId id, size_t channel, Packet packet);
	void sendData(PeerId id, void * data, size_t dataSize);
	void sendDataByChannel(PeerId id, enet_uint8 channel, void * data, size_t dataSize);

	template<typename Packet>
	void bufferPacket(PeerId id, Packet packet);
	template<typename Packet>
	void bufferPacketToChannel(PeerId id, size_t channel, Packet packet);
	void bufferData(PeerId id, void* data, size_t dataSize);
	void bufferDataToChannel(PeerId id, enet_uint8 channel, void* data, size_t dataSize);

	void sendBuffered();

	int service(ENetEvent *event, enet_uint32 timeout);
	PeerId connect(const std::string & ip, int port, size_t channels);
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
};

template<typename Packet>
inline void Host::sendPacketByChannel(PeerId id, size_t channel, Packet packet) {
	if (peers.size() > id) {
		packet.serialize();
		enet_peer_send(peers[id], channel, enet_packet_create(&packet, sizeof(Packet), 0));
	}
}

template<typename Packet>
inline void Host::sendPacket(PeerId id, Packet packet) {
	if (peers.size() > id) {
		packet.serialize();
		enet_peer_send(peers[id], (channelIncrementer++) % channelCount, enet_packet_create(&packet, sizeof(Packet), 0));
	}
}

template<typename Packet>
inline void Host::bufferPacketToChannel(PeerId id, size_t channel, Packet packet) {
	packet.serialize();
	idDestinedPackets.emplace_back(DestinedPacket{ enet_packet_create(&packet, sizeof(Packet), 0), id, channel });
}

template<typename Packet>
inline void Host::bufferPacket(PeerId id, Packet packet) {
	packet.serialize();
	idDestinedPackets.emplace_back(DestinedPacket{ enet_packet_create(&packet, sizeof(Packet), 0), id, (channelIncrementer++) % channelCount });
}
