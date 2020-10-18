#pragma once
#include "enet/enet.h"
#include "Pool.h"

#include <string>
#include <deque>

using PeerId = uint64_t;

//packet with with info about how to send it
struct DestinedPacket {
	ENetPacket* packet;
	ENetPeer* peer;
	PeerId id;
	size_t channel;
};

class Host {
public:
	Host();
	~Host();
	bool createClient(size_t peerCount, size_t channels = 0, enet_uint32 incomingBandwidth = 0, enet_uint32 outgoingBandwidth = 0);
	bool createServer(int port, size_t peerCount, size_t channels = 0, enet_uint32 incomingBandwidth = 0, enet_uint32 outgoingBandwidth = 0);
	template<typename Packet>
	void sendPacket(ENetPeer * peer, size_t channel, Packet packet);
	void sendData(ENetPeer * peer, enet_uint8 channel, void * data, size_t dataSize);

	template<typename Packet>
	void sendPacket(PeerId id, size_t channel, Packet packet);
	void sendData(PeerId id, enet_uint8 channel, void * data, size_t dataSize);

	template<typename Packet>
	void bufferPacket(ENetPeer* peer, size_t channel, Packet packet);
	void bufferData(ENetPeer* peer, enet_uint8 channel, void* data, size_t dataSize);

	template<typename Packet>
	void bufferPacket(PeerId id, size_t channel, Packet packet);
	void bufferData(PeerId id, enet_uint8 channel, void* data, size_t dataSize);

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
	Pool<ENetPeer *> peers;
	std::deque<PeerId> freeIds;
	std::vector<DestinedPacket> peerDestinedPackets;
	std::vector<DestinedPacket> idDestinedPackets;
};

template<typename Packet>
void Host::sendPacket(ENetPeer * peer, size_t channel, Packet packet) {
	packet.serialize();
	enet_peer_send(peer, channel, enet_packet_create(&packet, sizeof(Packet), 0));
}

template<typename Packet>
inline void Host::sendPacket(PeerId id, size_t channel, Packet packet) {
	if (peers.size() > id) {
		packet.serialize();
		enet_peer_send(peers[id], channel, enet_packet_create(&packet, sizeof(Packet), 0));
	}
}

template<typename Packet>
inline void Host::bufferPacket(ENetPeer* peer, size_t channel, Packet packet) {
	packet.serialize();
	peerDestinedPackets.emplace_back(DestinedPacket{ enet_packet_create(&packet, sizeof(Packet), 0), peer, 0, channel });
}

template<typename Packet>
inline void Host::bufferPacket(PeerId id, size_t channel, Packet packet) {
	packet.serialize();
	idDestinedPackets.emplace_back(DestinedPacket{ enet_packet_create(&packet, sizeof(Packet), 0), nullptr, id, channel });
}
