#include "Host.h"
#include "Game.h"
#include <iostream>
#include <algorithm>

Host::Host(size_t channelCount_) :
	host{NULL},
	channelCount{channelCount_},
	channelIncrementer{0},
	clientConnected{false},
	connectCallback{},
	connectedPeers{}
{
}

Host::~Host() {
	enet_host_destroy(host);
}

void Host::createClient(size_t peerCount, size_t channels, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth) {
	host = enet_host_create(NULL, peerCount, channels, incomingBandwidth, outgoingBandwidth);
	//unable to generate host
	if (host == NULL) throw std::exception{};

	channelCount = channels;
	ownedNetIds.resize(peerCount);
	connectedPeers.resize(peerCount);
	type = Type::client;
}

void Host::createServer(int port, size_t peerCount, size_t channels, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth) {
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;
	host = enet_host_create(&address, peerCount, channels, incomingBandwidth, outgoingBandwidth);
	//unable to generate host
	if (host == NULL) throw std::exception{};

	channelCount = channels;
	ownedNetIds.resize(peerCount);
	connectedPeers.resize(peerCount);
	type = Type::server;
}

void Host::sendAllData(const ByteStream& data) {
	enet_host_broadcast(host, (channelIncrementer++) % channelCount, enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Host::sendAllDataByChannel(enet_uint8 channel, const ByteStream& data) {
	enet_host_broadcast(host, channel, enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Host::bufferAllData(const ByteStream& data) {
	idDestinedPackets.emplace_back(DestinedPacket{ enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE), 0, (channelIncrementer++) % channelCount, true});
}

void Host::bufferAllDataByChannel(enet_uint8 channel, const ByteStream& data) {
	idDestinedPackets.emplace_back(DestinedPacket{ enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE), 0, channel, true});
}

void Host::sendData(PeerId id, const ByteStream& data) {
	enet_peer_send(host->peers + id, (channelIncrementer++) % channelCount, enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Host::sendDataByChannel(PeerId id, enet_uint8 channel, const ByteStream& data) {
	enet_peer_send(host->peers + id, channel, enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Host::bufferData(PeerId id, const ByteStream& data) {
	idDestinedPackets.emplace_back(DestinedPacket{ enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE), id, (channelIncrementer++) % channelCount, false});
}

void Host::bufferDataToChannel(PeerId id, enet_uint8 channel, const ByteStream& data) {
	idDestinedPackets.emplace_back(DestinedPacket{ enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE), id, channel, false});
}

void Host::sendBuffered() {
	for (auto& packet : idDestinedPackets) {
		if (packet.broadcast) {
			enet_host_broadcast(host, packet.channel, packet.packet);
		}
		else {
			enet_peer_send(host->peers + packet.id, packet.channel, packet.packet);
		}
	}
	idDestinedPackets.clear();
}

void Host::handlePackets(Game& game) {

	ENetEvent e;
	PeerId connectedId, disconnectedId;
	while (service(&e, 0) > 0) {
		switch (e.type) {
		case ENET_EVENT_TYPE_CONNECT:
			connectedId = getId(e.peer);
			connectedPeers.set(connectedId, true);
			std::cout << "Connection received from new peer " << connectedId << '\n';

			// this might need to be moved
			if (type == Type::client) {
				clientConnected = true;
				ByteStream pingPacket;
				pingPacket << Packet::PingId;
				pingPacket << game.getGameTick();
				bufferAllDataByChannel(0, pingPacket);
			}

			if (connectCallback) connectCallback(game, connectedId);
			else std::cout << "Connection received. No callback function implemented.\n";
			game.onConnect(connectedId);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			if(type == Type::client) clientConnected = false;
			disconnectedId = getId(e.peer);
			connectedPeers.set(disconnectedId, false);
			if (disconnectCallback) disconnectCallback(game, disconnectedId);
			else std::cout << "Disconnected " << disconnectedId << ". No callback function implemented.\n";
			game.onDisconnect(disconnectedId);
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			ByteStream stream;
			stream.putData(e.packet->data, e.packet->dataLength);
			PacketId id;
			stream.peek(id);

			if (packetHandlers.find(id) != packetHandlers.end()) {
				packetHandlers.at(id)->handlePacket(game, stream, getId(e.peer));
			}
			else {
				std::cout << "No handler for ID \'" << id << "\'.\n";
			}
			break;
		}
	}
}

int Host::service(ENetEvent * event, enet_uint32 timeout) {
	return enet_host_service(host, event, timeout);
}

void Host::tryConnect(const std::string & ip, int port, size_t channels) {
	ENetAddress address;
	enet_address_set_host(&address, ip.c_str());
	address.port = port;
	enet_host_connect(host, &address, channels, 0);
}

ENetPeer& Host::getPeer(PeerId id) {
	if (id >= 0 && id < host->connectedPeers) {
		return host->peers[id];
	}
	//uh oh, we couldn't find it
	throw std::exception{};
}

PeerId Host::getId(ENetPeer* peer) {
	return peer - host->peers;
}

void Host::disconnect(PeerId peerId) {
	enet_peer_disconnect(host->peers + peerId, 0);
	connectedPeers.set(peerId, false);
}

void Host::resetConnection(PeerId peerId) {
	enet_peer_reset(host->peers + peerId);
	connectedPeers.set(peerId, false);
}

void Host::setConnectCallback(std::function<ConnectCallback> newCallBack) {
	connectCallback = newCallBack;
}

void Host::setDisconnectCallback(std::function<DisconnectCallback> newCallBack) {
	disconnectCallback = newCallBack;
}

bool Host::isConnected() {
	return clientConnected;
}

void Host::addNetIdToPeer(PeerId peerId, NetworkId netId) {
	ownedNetIds[peerId].emplace_back(netId);
}

void Host::removeNetIdFromPeer(PeerId peerId, NetworkId netId) {
	ownedNetIds[peerId].erase(std::remove(ownedNetIds[peerId].begin(), ownedNetIds[peerId].end(), netId), ownedNetIds[peerId].end());
}

const std::vector<NetworkId>& Host::getPeerOwnedNetIds(PeerId id) {
	return ownedNetIds[id];
}
bool Host::isPeerConnected(PeerId id) {
	return connectedPeers[id];
}

size_t Host::getConnectedPeerCount() {
	return host->connectedPeers;
}

size_t Host::getPeerCount() {
	return host->peerCount;
}