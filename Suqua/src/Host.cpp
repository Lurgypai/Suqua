
#include "Host.h"

Host::Host(size_t channelCount_) :
	host{NULL},
	channelCount{channelCount_},
	channelIncrementer{0},
	connected{false}
{}

Host::~Host() {
	enet_host_destroy(host);
	for (auto iter = peers.beginResource(); iter != peers.endResource(); ++iter) {
		if (!iter->isFree) {
			enet_peer_reset(iter->val);
		}
	}
}

bool Host::createClient(size_t peerCount, size_t channels, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth) {
	host = enet_host_create(NULL, peerCount, channels, incomingBandwidth, outgoingBandwidth);

	if (host != NULL) {
		channelCount = channels;
		peers.resize(peerCount);
		for (size_t id = 0; id != peerCount;) {
			freeIds.push_back(id++);
		}
	}

	return host != NULL;
}

bool Host::createServer(int port, size_t peerCount, size_t channels, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth) {
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;
	host = enet_host_create(&address, peerCount, channels, incomingBandwidth, outgoingBandwidth);

	if (host != NULL) {
		channelCount = channels;
		peers.resize(peerCount);
		for (size_t id = 0; id != peerCount;) {
			freeIds.push_back(++id);
		}
	}

	return host != NULL;
}

void Host::sendAllData(const ByteStream& data) {
	enet_host_broadcast(host, (channelIncrementer++) % channelCount, enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Host::sendAllDataByChannel(enet_uint8 channel, const ByteStream& data) {
	enet_host_broadcast(host, channel, enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Host::bufferAllData(const ByteStream& data) {
	idDestinedPackets.emplace_back(DestinedPacket{ enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE), 0, (channelIncrementer++) % channelCount });
}

void Host::bufferAllDataByChannel(enet_uint8 channel, const ByteStream& data) {
	idDestinedPackets.emplace_back(DestinedPacket{ enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE), 0, channel });
}

void Host::sendData(PeerId id, const ByteStream& data) {
	enet_peer_send(peers[id], (channelIncrementer++) % channelCount, enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Host::sendDataByChannel(PeerId id, enet_uint8 channel, const ByteStream& data) {
	enet_peer_send(peers[id], channel, enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Host::bufferData(PeerId id, const ByteStream& data) {
	idDestinedPackets.emplace_back(DestinedPacket{ enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE), id, (channelIncrementer++) % channelCount });
}

void Host::bufferDataToChannel(PeerId id, enet_uint8 channel, const ByteStream& data) {
	idDestinedPackets.emplace_back(DestinedPacket{ enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE), id, channel });
}

void Host::sendBuffered() {
	for (auto& packet : idDestinedPackets) {
		if (packet.id == 0) {
			enet_host_broadcast(host, packet.channel, packet.packet);
		}
		else {
			enet_peer_send(peers[packet.id], packet.channel, packet.packet);
		}
	}
	idDestinedPackets.clear();
}

void Host::handlePackets(Game& game) {
	if (!connected) {
		ENetEvent e;
		while (service(&e, 0) > 0) {
			if (e.type == ENET_EVENT_TYPE_CONNECT) {
				connected = true;
				break;
			}
		}
	}
	if (connected) {
		ENetEvent e;
		while (service(&e, 0) > 0) {
			switch (e.type) {
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				connected = false;
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				ByteStream stream;
				stream.writeData(e.packet->data, e.packet->dataLength);
				PacketHandlerId id;
				stream >> id;
				packetHandlers.at(id)->handlePacket(stream, game);
				break;
			}
		}
	}
}

int Host::service(ENetEvent * event, enet_uint32 timeout) {
	return enet_host_service(host, event, timeout);
}

PeerId Host::tryConnect(const std::string & ip, int port, size_t channels) {
	if (!freeIds.empty()) {
		ENetAddress address;
		enet_address_set_host(&address, ip.c_str());
		address.port = port;

		PeerId id = freeIds.front();
		freeIds.pop_front();
		peers.add(id, enet_host_connect(host, &address, channels, 0));
		return id;
	}
}

PeerId Host::addPeer(ENetPeer * peer) {
	if (!freeIds.empty()) {
		PeerId id = freeIds.front();
		freeIds.pop_front();
		peers.add(id, peer);
		return id;
	}
}

void Host::removePeer(PeerId peerId) {
	freeIds.push_back(peerId);
	peers.free(peerId);
}

void Host::disconnect(PeerId peerId) {
	enet_peer_disconnect(peers[peerId], 0);
	removePeer(peerId);
}

void Host::resetConnection(PeerId peerId) {
	enet_peer_reset(peers[peerId]);
	removePeer(peerId);
}