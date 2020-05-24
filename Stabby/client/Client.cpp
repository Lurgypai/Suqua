
#include <numeric>
#include <iostream>

#include "../player/ClientPlayerComponent.h"
#include "../player/OnlinePlayerLC.h"
#include "../graphics/PlayerGC.h"
#include "../graphics/CapturePointGC.h"

#include "DebugIO.h"
#include "Client.h"
#include "EntitySystem.h"
#include "EntityBaseComponent.h"
#include "ControllerComponent.h"

Client::Client() :
	clientTime{0}
{
	enet_initialize();
	DebugIO::printLine("Starting client.");
	if (!client.createClient(1, 3)) {
		DebugIO::printLine("Error while trying to create client.");
		return;
	}
	connected = false;
}

Client::~Client() {
	enet_deinitialize();
}

void Client::connect(const std::string & ip, int port) {
	if (!connected) {
		serverId = client.connect(ip, port, 3);

		ENetEvent e;
		if (client.service(&e, 5000) > 0 && e.type == ENET_EVENT_TYPE_CONNECT) {
			DebugIO::printLine("Connection established.");
		}

		//attempt to get the time, as well as the networkid.

		bool handshakeComplete{ false };
		while (client.service(&e, 5000) > 0) {
			switch (e.type) {
			case ENET_EVENT_TYPE_DISCONNECT:
				DebugIO::printLine("Disconnected from server.");
				client.removePeer(serverId);
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				//this portion will denote a welcome packet
				if (PacketUtil::readPacketKey(e.packet) == WELCOME_KEY) {
					WelcomePacket packet{};
					std::memcpy(&packet, e.packet->data, e.packet->dataLength);
					packet.unserialize();
					DebugIO::printLine("Handshake Complete!");
					DebugIO::printLine("Time is " + std::to_string(packet.currentTick) + ", and our ID is " + std::to_string(packet.netId) + "!");
					networkTime = packet.currentTick;
					handshakeComplete = true;

					online->registerOnlineComponent(playerId, packet.netId);
				}
				break;
			}
			if (handshakeComplete) {
				break;
			}
		}
		if (!handshakeComplete) {
			DebugIO::printLine("Unable to complete handshake, disconnecting.");
			client.disconnect(serverId);
		}
		else {
			ping();
			connected = true;
		}
	}
	else {
		DebugIO::printLine("We are already connected to a server.");
	}
}

void Client::send(size_t size, void* data) {
	client.sendData(serverId, 0, data, size);
}

void Client::service() {
	if (connected) {
		ENetEvent e;
		while (client.service(&e, 0) > 0) {
			switch (e.type) {
			case ENET_EVENT_TYPE_DISCONNECT:
				DebugIO::printLine("Disconnected from server.");
				connected = false;
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				receive(e);
				break;
			}
		}
	}
}



void Client::ping() {
	TimestampPacket p;
	OnlineComponent* online = EntitySystem::GetComp<OnlineComponent>(playerId);
	p.id = online->getNetId();
	p.clientTime = clientTime;
	client.sendPacket(serverId, 0, p);
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

void Client::setPlayer(EntityId id_) {
	playerId = id_;
}

bool Client::isBehindServer() {
	return behindServer;
}

void Client::resetBehindServer() {
	behindServer = false;
}

void Client::setWeaponManager(WeaponManager& weapons_) {
	weapons = &weapons_;
}

void Client::setClientPlayerSystem(ClientPlayerSystem* clientPlayer) {
	clientPlayers = clientPlayer;
}

void Client::setOnlineSystem(OnlineSystem* online_) {
	online = online_;
}

void Client::setMode(DominationMode* mode_) {
	mode = mode_;
}

void Client::setSpawns(SpawnSystem* spawns_) {
	spawns = spawns_;
}

const std::vector<NetworkId>& Client::getNewPlayers() const {
	return toJoinIds;
}

void Client::clearNewPlayers() {
	toJoinIds.clear();
}

const std::vector<CapturePointPacket>& Client::getMissingCapturePoints() const {
	return toMakeCapturePoins;
}

void Client::clearMissingCapturePoints() {
	toMakeCapturePoins.clear();
}

void Client::receive(ENetEvent & e) {
	std::string packetKey = PacketUtil::readPacketKey(e.packet);
	if (packetKey == JOIN_KEY) {
		JoinPacket p;
		PacketUtil::readInto<JoinPacket>(p, e.packet);
		p.unserialize();
		DebugIO::printLine("Player with id " + std::to_string(p.joinerId) + " has joined!");
		toJoinIds.push_back(p.joinerId);
	}
	else if (packetKey == STATE_KEY) {
		std::vector<StatePacket> states;
		size_t size = e.packet->dataLength / sizeof(StatePacket);
		states.resize(size);

		PacketUtil::readInto<StatePacket>(&states[0], e.packet, size);

		for (auto & p : states) {
			p.unserialize();
			EntityId targetId = online->getEntity(p.id);
			if (targetId != 0) {
				ClientPlayerComponent* player = EntitySystem::GetComp<ClientPlayerComponent>(targetId);
				if (player != nullptr) {
					clientPlayers->repredict(playerId, p.id, p.state, CLIENT_TIME_STEP);
				}
				else if (EntitySystem::Contains<OnlinePlayerLC>()) {
					auto onlinePlayer = EntitySystem::GetComp<OnlinePlayerLC>(targetId);
					if (onlinePlayer) {
						onlinePlayer->interp(p.state, p.when);
						ControllerComponent* onlinePlayerController = EntitySystem::GetComp<ControllerComponent>(targetId);
						onlinePlayerController->getController() = Controller{ p.controllerState };
					}
					else {
						DebugIO::printLine("Unable to find player " + std::to_string(p.id) + ". Did they disconnect?");
					}

				}
			}
		}
	}
	else if (packetKey == TEAM_KEY) {
		TeamChangePacket p;
		PacketUtil::readInto(p, e.packet);
		p.unserialize();
		
		EntitySystem::GetComp<CombatComponent>(playerId)->teamId = p.targetTeamId;
		DebugIO::printLine("Team switched to " + std::to_string(p.targetTeamId) + ".");
	}
	else if (packetKey == QUIT_KEY) {
		QuitPacket q;
		PacketUtil::readInto<QuitPacket>(q, e.packet);
		q.unserialize();
		EntityId targetEntity = online->getEntity(q.id);
		if (targetEntity != 0) {
			EntitySystem::GetComp<EntityBaseComponent>(targetEntity)->isDead = true;
		}

		DebugIO::printLine("Player " + std::to_string(q.id) + " has disconnected.");
	}
	else if (packetKey == TIME_KEY) {
		TimestampPacket p;
		PacketUtil::readInto<TimestampPacket>(p, e.packet);
		p.unserialize();
		Time_t latestRtt = clientTime - p.clientTime;
		recalculatePing(latestRtt);
		//kinda maybe synchronized
		networkTime = p.gameTime + (((static_cast<double>(currentPing) / 2) * CLIENT_TIME_STEP) / GAME_TIME_STEP);
	}
	else if (packetKey == WEAPON_KEY) {
		WeaponChangePacket p{};
		PacketUtil::readInto<WeaponChangePacket>(p, e.packet);
		p.unserialize();
		std::string attackId{};
		attackId.resize(p.size);
		std::memcpy(attackId.data(), e.packet->data + sizeof(WeaponChangePacket), p.size);

		EntityId playerId = online->getEntity(p.id);
		if (playerId != 0) {
			CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(playerId);
			combat->setAttack(attackId);

			PlayerGC* graphics = EntitySystem::GetComp<PlayerGC>(playerId);
			graphics->attackSprite = weapons->cloneAnimation(attackId);
		}
	}
	else if (packetKey == CAP_KEY) {
		std::vector<CapturePointPacket> capturePointPackets;
		size_t size = e.packet->dataLength / sizeof(CapturePointPacket);
		capturePointPackets.resize(size);

		PacketUtil::readInto<CapturePointPacket>(capturePointPackets.data(), e.packet, size);

		for (auto& packet : capturePointPackets) {
			packet.unserialize();

			EntityId targetId = online->getEntity(packet.netId);
			if (EntitySystem::Contains<CapturePointComponent>() && targetId != 0) {
				CapturePointComponent* capturePoint = EntitySystem::GetComp<CapturePointComponent>(targetId);
				capturePoint->setState(packet.state);
				spawns->assignTeam(targetId, packet.state.currTeamId);
			}
			else {
				toMakeCapturePoins.push_back(packet);
			}
		}
	}
	else if (packetKey == MESSAGE_KEY) {
		MessagePacket message{};
		PacketUtil::readInto<MessagePacket>(message, e.packet);
		message.unserialize();

		DebugIO::printLine(std::string{ message.message });
	}
}
