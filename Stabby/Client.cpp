#include "pch.h"
#include <numeric>
#include "NetworkTypes.h"

#include "DebugIO.h"
#include "Client.h"
#include "EntitySystem.h"
#include "OnlinePlayerLC.h"
#include "PlayerGC.h"
#include "ClientPlayerLC.h"
#include "ServerClientData.h"

#include <iostream>

Client::Client() :
	id{ 0 }
{
	enet_initialize();
	DebugIO::printLine("Starting client.");
	client = enet_host_create(NULL, 1, 2, 0, 0);
	if (client == NULL) {
		DebugIO::printLine("Error while trying to create client.");
		return;
	}
	connected = false;
}

Client::~Client() {
	if(peer != nullptr)
		enet_peer_disconnect(peer, 0);
	enet_host_destroy(client);
	enet_deinitialize();
}

void Client::connect(Time_t now, const std::string & ip, int port) {
	if (!connected) {
		ENetAddress address;
		enet_address_set_host(&address, ip.c_str());
		address.port = port;

		peer = enet_host_connect(client, &address, 2, 0);
		if (peer == NULL) {
			DebugIO::printLine("Unable to establish connection.");
		}

		ENetEvent e;
		if (enet_host_service(client, &e, 5000) > 0 && e.type == ENET_EVENT_TYPE_CONNECT) {
			DebugIO::printLine("Connection established.");
		}

		//attempt to get the time, as well as the networkid.

		bool handshakeComplete{ false };
		while (enet_host_service(client, &e, 5000) > 0) {
			switch (e.type) {
			case ENET_EVENT_TYPE_DISCONNECT:
				DebugIO::printLine("Disconnected from server.");
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				//this portion will denote a welcome packet
				if (PacketUtil::readPacketKey(e.packet) == WELCOME_KEY) {
					WelcomePacket packet{};
					std::memcpy(&packet, e.packet->data, e.packet->dataLength);
					DebugIO::printLine("Handshake Complete!");
					DebugIO::printLine("Time is " + std::to_string(packet.currentTick) + ", and our ID is " + std::to_string(packet.netId) + "!");
					networkTime = packet.currentTick;
					id = packet.netId;
					handshakeComplete = true;
				}
				break;
			}
			if (handshakeComplete) {
				break;
			}
		}
		if (!handshakeComplete) {
			DebugIO::printLine("Unable to complete handshake, disconnecting.");
			enet_peer_disconnect(peer, 0);
		}
		else {
			TimestampPacket p;
			p.clientTime = now;
			p.id = id;
			enet_peer_send(peer, 0, enet_packet_create(&p, sizeof(TimestampPacket), 0));
			connected = true;
		}
	}
	else {
		DebugIO::printLine("We are already connected to a server.");
	}
}

void Client::send(ENetPacket * p) {
	enet_peer_send(peer, 0, p);
}

void Client::service(Time_t now_) {
	now = now_;
	if (connected) {
		ENetEvent e;
		while (enet_host_service(client, &e, 0) > 0) {
			switch (e.type) {
			case ENET_EVENT_TYPE_DISCONNECT:
				DebugIO::printLine("Disconnected from server.");
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				receive(e);
				break;
			}
		}
	}
}



void Client::ping(Time_t now) {
	TimestampPacket p;
	p.clientTime = now;
	enet_peer_send(peer, 0, enet_packet_create(&p, sizeof(TimestampPacket), 0));
}

void Client::recalculatePing(Time_t nextPing) {
	if (pings.size() >= PING_COUNT)
		pings.pop_front();
	pings.push_back(nextPing);

	currentPing = (std::accumulate(pings.begin(), pings.end(), 0)) / pings.size();
}

unsigned int Client::getPing() {
	return currentPing * CLIENT_TIME_STEP * 100;
}

bool Client::getConnected() {
	return connected;
}

Time_t Client::getTime() {
	return networkTime;
}

void Client::progressTime(Time_t delta) {
	networkTime += delta;
}

NetworkId Client::getNetId() {
	return id;
}

void Client::setPlayer(EntityId id_) {
	playerId = id_;
}

void Client::receive(ENetEvent & e) {
	std::string packetKey = PacketUtil::readPacketKey(e.packet);
	std::vector<NetworkId> ids;
	if (packetKey == JOIN_KEY) {
		JoinPacket p;
		PacketUtil::readInto<JoinPacket>(p, e.packet);
		DebugIO::printLine("Player with id " + std::to_string(p.joinerId) + " has joined!");
		ids.push_back(p.joinerId);
	}

	else if (packetKey == STATE_KEY) {
		std::vector<StatePacket> states;
		size_t size = e.packet->dataLength / sizeof(StatePacket);
		states.resize(size);

		PacketUtil::readInto<StatePacket>(&states[0], e.packet, size);

		Pool<OnlinePlayerLC> * onlinePlayers = EntitySystem::GetPool<OnlinePlayerLC>();

		for (auto & p : states) {
			if (onlinePlayers != nullptr) {
				for (auto& onlinePlayer : *onlinePlayers) {
					if ((!onlinePlayer.isFree) && onlinePlayer.val.getNetId() == p.id) {
						onlinePlayer.val.interp(p.state, p.when);
					}
				}
			}
			if (id == p.id) {
				ClientPlayerLC * player = EntitySystem::GetComp<ClientPlayerLC>(playerId);
				if (player != nullptr) {
					player->repredict(p.state);
				}
			}
		}

		/*
		StatePacket p;
		PacketUtil::readInto<StatePacket>(p, e.packet);
		Pool<OnlinePlayerLC> * onlinePlayers = EntitySystem::GetPool<OnlinePlayerLC>();
		if (onlinePlayers != nullptr) {
			for (auto& onlinePlayer : *onlinePlayers) {
				if ((!onlinePlayer.isFree) && onlinePlayer.val.getNetId() == p.id) {
					onlinePlayer.val.interp(p.state, p.when);
				}
			}
		}
		if (id == p.id) {
			ClientPlayerLC * player = EntitySystem::GetComp<ClientPlayerLC>(playerId);
			if (player != nullptr) {
				player->repredict(p.state);
			}
		}
		*/
	}
	else if (packetKey == QUIT_KEY) {
		QuitPacket q;
		PacketUtil::readInto<QuitPacket>(q, e.packet);
		for (auto& onlinePlayer : *EntitySystem::GetPool<OnlinePlayerLC>()) {
			if (!onlinePlayer.isFree && onlinePlayer.val.getNetId() == q.id) {
				//remove
				onlinePlayer.isFree = true;
				EntityId id = onlinePlayer.val.getId();
				EntitySystem::FreeComps<PlayerGC>(1, &id);
			}
		}
		DebugIO::printLine("Player " + std::to_string(q.id) + " has disconnected.");
	}
	else if (packetKey == TIME_KEY) {
		TimestampPacket p;
		PacketUtil::readInto<TimestampPacket>(p, e.packet);
		Time_t latestRtt = now - p.clientTime;
		recalculatePing(latestRtt);
		//kinda maybe synchronized
		networkTime = p.gameTime + (((static_cast<double>(currentPing) / 2) * CLIENT_TIME_STEP) / GAME_TIME_STEP);
	}

	if (!ids.empty()) {
		std::vector<EntityId> entities;
		entities.resize(ids.size());
		EntitySystem::GenEntities(ids.size(), &entities[0]);
		EntitySystem::MakeComps<OnlinePlayerLC>(entities.size(), &entities[0]);
		EntitySystem::MakeComps<PlayerGC>(entities.size(), &entities[0]);

		for (int i = 0; i != ids.size(); ++i) {
			EntityId entity = entities[i];
			NetworkId netId = ids[i];
			EntitySystem::GetComp<PlayerGC>(entity)->load("images/evil_stabbyman.png");
			EntitySystem::GetComp<OnlinePlayerLC>(entity)->setNetId(netId);
		}
	}
}
