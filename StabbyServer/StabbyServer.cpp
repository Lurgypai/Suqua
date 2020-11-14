// StabbyServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <fstream>

#include "enet/enet.h"

#include "SDL.h"
#include "Controller.h"
#include <list>
#include <algorithm>
#include <vector>
#include <cstring>
#include <unordered_map>

#include "stage.h"

#include "PhysicsSystem.h"
#include "EntityBaseComponent.h"
#include "DebugFIO.h"
#include "DebugIO.h"
#include "MarkedStream.h"


#include "server/User.h"
#include "server/Settings.h"
#include "player/ServerPlayerSystem.h"
#include "player.h"
#include "gamestate.h"

#include "gamemode.h"
#include "nlohmann/json.hpp"

#define CLIENT_SIDE_DELTA 1.0 / 120

//I don't know how to copy peers, or If I should even try. Thus, to simplifiy things copying of connections is not allowed,
//to ensure peer->data isn't deleted multiple times

using UserPtr = std::unique_ptr<User>;
using std::ifstream;
using std::string;
using std::cout;
using json = nlohmann::json;

int main(int argv, char* argc[])
{
	SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER);
	cout << "Starting server...\n";
	enet_initialize();

	ifstream file{ "settings.json" };
	if (!file.good()) {
		cout << "Unable to open 'settings' file. Closing server.\n";
		return 1;
	}

	string line;
	json settings{};
	settings << file;
	int port = settings["port"];
	double disconnectDelay = settings["disconnectDelay"];
	double forceDisconnectDelay = settings["forceDisconnectDelay"];
	int aiPlayerCount = settings["aiPlayerCount"];

	std::string stageName = settings["stage"];

	SpawnSystem spawns;
	Stage stage{stageName, spawns};

	Host server;
	if (!server.createServer(port, 32, CHANNEL_COUNT)) {
		std::cout << "Oops, no server! Perhaps one is already running on this port?\n";
		return -1;
	}

	std::cout << "Beginning server loop.\n";

	Uint64 prev = SDL_GetPerformanceCounter();

	ENetPacket* packet;

	//current tick in server time
	Time_t currentTick{ 0 };

	//the current game time
	Time_t gameTime{ 0 };
	//the last time we were updated to
	Time_t lastUpdatedTime{ 0 };

	std::vector<ControllerPacket> ctrls;
	std::unordered_map<PeerId, UserPtr> users;

	PhysicsSystem physics{};
	physics.runPhysics(CLIENT_TIME_STEP);
	CombatSystem combat{};
	WeaponManager weapons{};
	ClimbableSystem climbables{};
	PlayerManager players{};
	ServerPlayerSystem onlinePlayers{};
	DominationMode mode{};
	OnlineSystem online{};
	GameStateId currGameState{ 0 };
	RespawnSystem respawner{&spawns};

	std::unordered_map<EntityId, PlayerState> prevPlayerStates;
	mode.load(&spawns, 2, 1, 144000);

	for (auto& capturePoint : EntitySystem::GetPool<CapturePointComponent>()) {
		online.addOnlineComponent(capturePoint.getId());
	}

	weapons.loadAttacks("attacks/hit");
	climbables.updateClimbables();

	PeerId clientPeerId = 0;
	DebugFIO::AddFOut("s_out.txt");
	DebugFIO::AddFOut("plr_log_s.txt");
	DebugFIO::Out("plr_log_s.txt") << "Client Time\tNetId\tControllerState\tPrev Controller State\tPos\tVel\tState\tRoll Frame\tActive Attack\tAttack Frame\tHealth\tStun Frame\tFacing\tSpawnPoint\tAttack Freeze Frame\tFrozen\tAttack Speed\tMove Speed\tHeal Frame\tHeal Delay\tTeam Id\tStamina\tStamina Recharge Frame\tDeath Frame\tWeapon Tag\tUser Tag"; // make it output the player states as a tab separated thing

	//generate player buffer
	std::vector<EntityId> aiPlayers(aiPlayerCount, 0);
	for (auto& id : aiPlayers) {
		id = players.makePlayer(weapons);
		EntitySystem::MakeComps<AIPlayerComponent>(1, &id);
		EntitySystem::GetComp<NameTagComponent>(id)->nameTag = "AI";

		mode.addPlayer(id);
		online.addOnlineComponent(id);
		PlayerLC* player = EntitySystem::GetComp<PlayerLC>(id);
		player->chooseSpawn();
	}

	//switch game to tick at client speed, but only send updates out at server speed
	//test this, it looks like its done.
	while (true) {

		//service packets as fast as we can
		ENetEvent event;
		int packetsPolled = 0;
		while (server.service(&event, 0) > 0) {
			switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT: {
				std::cout << "Connection received.\n";
				clientPeerId = server.addPeer(event.peer);

				users.emplace(clientPeerId, std::make_unique<User>(User{&players, &weapons, clientPeerId, std::make_unique<Connection>(*event.peer, clientPeerId, currentTick) }));
				auto& newUser = users[clientPeerId];
				newUser->getPlayer().chooseSpawn();
				online.addOnlineComponent(newUser->getId());

				NetworkId newClientNetId = newUser->getOnline().getNetId();

				mode.addPlayer(newUser->getId());

				WelcomePacket welcomePacket;
				welcomePacket.currentTick = gameTime;
				welcomePacket.netId = newClientNetId;
				server.sendPacket<WelcomePacket>(clientPeerId, welcomePacket);

				//notify all online players of a new players join
				for (auto& pair : users) {
					if (pair.first != clientPeerId) {
						//tell them about us
						JoinPacket join{};
						join.joinerId = newClientNetId;
						server.sendPacket<JoinPacket>(pair.second->getPeerId(), join);
					}
				}
				//fill us in about all players
				MarkedStream m;
				for (auto& playerLC : EntitySystem::GetPool<PlayerLC>()) {
					OnlineComponent* online = EntitySystem::GetComp<OnlineComponent>(playerLC.getId());
					if (online->getNetId() != newClientNetId) {
						//tell us about them
						JoinPacket us{};
						us.joinerId = online->getNetId();
						server.sendPacket(clientPeerId, us);
					}
				}

				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
				if (event.packet->dataLength == 0) {
					std::cout << "Garbage packet received.\n";
				}
				else {
					PeerId senderId = *static_cast<NetworkId*>(event.peer->data);
					User* sender = users[senderId].get();

					if (sender == nullptr)
						std::cout << "Sender was not found.\n";
					sender->getConnection()->setLastPacket(currentTick);

					std::string key = PacketUtil::readPacketKey(event.packet);
					if (key == CONT_KEY) {
						ControllerPacket cont{};
						std::memcpy(&cont, event.packet->data, event.packet->dataLength);
						cont.unserialize();

						ClientCommand comm{ Controller{ cont.state, cont.prevState }, cont.clientTime, cont.when };
						DebugFIO::Out("s_out.txt") << "Received and storing input " << static_cast<int>(cont.state) << ", " << static_cast<int>(cont.prevState) << " for time " << cont.clientTime << '\n';
						sender->getServerPlayer().bufferInput(comm);
						if (cont.clientTime < sender->getServerPlayer().getClientTime()) {
							ClientDelayedPacket delayPacket{};
							server.sendPacket(senderId, delayPacket);
						}
					}
					else if (key == TIME_KEY) {
						TimestampPacket time{};
						PacketUtil::readInto<TimestampPacket>(time, event.packet);
						time.unserialize();

						if (!sender->getServerPlayer().getTimeIsSet()) {
							sender->getServerPlayer().setTime(time.clientTime);
						}

						time.gameTime = gameTime;
						time.serverTime = currentTick;
						server.bufferPacket<TimestampPacket>(sender->getPeerId(), time);
						break;
					}
					else if (key == WEAPON_KEY) {
						WeaponChangePacket p{};
						PacketUtil::readInto<WeaponChangePacket>(p, event.packet);
						p.unserialize();
						std::string attackId{};
						attackId.resize(p.size);

						std::memcpy(attackId.data(), (event.packet->data) + sizeof(WeaponChangePacket), p.size);

						//if we have the weapon, set it and tell everyone
						if (weapons.hasWeapon(attackId)) {
							sender->getCombat().setAttack(attackId);

							for (auto& pair : users) {
								WeaponChangePacket ret;
								ret.size = attackId.size();
								ret.id = p.id;
								ret.serialize();
								char* data = static_cast<char*>(malloc(sizeof(WeaponChangePacket) + attackId.size()));
								memcpy(data, &ret, sizeof(WeaponChangePacket));
								memcpy(data + sizeof(WeaponChangePacket), attackId.data(), p.size);
								server.sendData(pair.first, data, sizeof(WeaponChangePacket) + attackId.size());
								free(data);
							}
						}
						//otherwise, tell the sender that we dont' have it
						else {
							attackId = sender->getCombat().getAttack().getId();
							WeaponChangePacket ret;
							ret.size = attackId.size();
							ret.id = p.id;
							ret.serialize();
							char* data = static_cast<char*>(malloc(sizeof(WeaponChangePacket) + attackId.size()));
							memcpy(data, &ret, sizeof(WeaponChangePacket));
							memcpy(data + sizeof(WeaponChangePacket), attackId.data(), p.size);
							server.sendData(senderId, data, sizeof(WeaponChangePacket) + attackId.size());
							free(data);
						}
					}
					else if (key == TEAM_KEY) {
						TeamChangePacket p;
						PacketUtil::readInto<TeamChangePacket>(p, event.packet);
						p.unserialize();
						server.bufferPacket(senderId, p);

						sender->getCombat().teamId = p.targetTeamId;
					}
					else if (key == ACK_KEY) {
						AcknowledgePacket p;
						PacketUtil::readInto<AcknowledgePacket>(p, event.packet);
						p.unserialize();

						sender->getServerPlayer().acknowledgeState(p.stateId);
					}
					else if (key == NAMETAG_KEY) {
						NameTagPacket p;
						PacketUtil::readInto<NameTagPacket>(p, event.packet);
						p.unserialize();

						sender->getNameTag().nameTag = p.nameTag;
					}
					else if (key == RES_KEY) {
						RespawnRequestPacket p;
						PacketUtil::readInto<RespawnRequestPacket>(p, event.packet);
						p.unserialize();

						EntityId spawnZone = online.getEntity(p.targetRespawnComp);
						SpawnComponent* spawnComp = EntitySystem::GetComp<SpawnComponent>(spawnZone);
						if (sender->getPlayer().getState().state == State::dead) {
							sender->getPlayer().respawn(spawnComp->findSpawnPos());
						}
					}

				}
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				QuitPacket q;
				PeerId disconnectPeerId;
				std::memcpy(&disconnectPeerId, event.peer->data, sizeof(disconnectPeerId));

				bool foundUser;

				if (users.find(disconnectPeerId) != users.end()) {

					auto& disconnectedUser = users[disconnectPeerId];

					std::cout << "Player " << disconnectPeerId << ", " << disconnectedUser->getOnline().getNetId() << " disconnected.\n";
					q.id = disconnectedUser->getOnline().getNetId();
					disconnectedUser->getConnection()->setShouldReset(true);

					for (auto& pair : users) {
						if (pair.first != disconnectPeerId) {
							server.sendPacket<QuitPacket>(pair.first, q);
						}
					}
				}
				else {
					throw std::exception{};
				}
			}
			break;
			default:
				std::cout << "Event handled.\n";
			}
			++packetsPolled;
		}

		Time_t now = SDL_GetPerformanceCounter();
		if (static_cast<double>(now - prev) / SDL_GetPerformanceFrequency() >= SERVER_TIME_STEP) {
			gameTime += SERVER_TIME_STEP / GAME_TIME_STEP;
			prev = now;

			server.sendBuffered();

			//std::cout << "Server tick, polling packets...\n";

			if (EntitySystem::Contains<PlayerLC>()) {
				for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
					prevPlayerStates[player.getId()] = player.getState();
				}
			}

			while (lastUpdatedTime != gameTime) {
				++lastUpdatedTime;

				physics.runPhysics(CLIENT_TIME_STEP);
				combat.runAttackCheck(CLIENT_TIME_STEP);

				//update all ai players
				if (EntitySystem::Contains<AIPlayerComponent>()) {
					for (auto& aiPlayer : EntitySystem::GetPool<AIPlayerComponent>()) {
						aiPlayer.update();
						players.update(aiPlayer.getId(), CLIENT_TIME_STEP, stage, spawns);
					}
				}

				onlinePlayers.updatePlayers(players, CLIENT_TIME_STEP, stage, spawns);
				respawner.updateAll();

				for (auto& pair : users) {
					PlayerState state = pair.second->getPlayer().getState();
					OnlineComponent& online = pair.second->getOnline();
					ControllerComponent& controller = pair.second->getController();
					DebugFIO::Out("plr_log_s.txt") << state.clientTime << '\t';
					DebugFIO::Out("plr_log_s.txt") << online.getNetId() << '\t';
					DebugFIO::Out("plr_log_s.txt") << static_cast<int>(controller.getController().getState()) << '\t';
					DebugFIO::Out("plr_log_s.txt") << static_cast<int>(controller.getController().getPrevState()) << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.pos << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.vel << '\t';
					DebugFIO::Out("plr_log_s.txt") << static_cast<int>(state.state) << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.rollFrame << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.activeAttack << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.attackFrame << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.health << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.stunFrame << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.facing << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.spawnPoint << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.attackFreezeFrame << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.frozen << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.attackSpeed << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.moveSpeed << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.healFrame << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.healDelay << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.teamId << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.stamina << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.staminaRechargeFrame << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.deathFrame << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.weaponTag << '\t';
					DebugFIO::Out("plr_log_s.txt") << state.userTag << '\t';
					DebugFIO::Out("plr_log_s.txt") << '\n';
				}

				/*
				DebugFIO::Out("s_out.txt") << "Current Game State: " << currGameState + 1 << '\n';
				for (auto& user : users) {
					DebugFIO::Out("s_out.txt") << "Player: " << user->getOnline().getNetId() << '\n';
					PlayerState state = user->getPlayer().getState();
					DebugFIO::Out("s_out.txt") << "plrState: " << static_cast<unsigned int>(state.state) << '\n';
					DebugFIO::Out("s_out.txt") << "rollFrame: " << state.rollFrame << '\n';
					DebugFIO::Out("s_out.txt") << "activeAttack: " << state.activeAttack << '\n';
					DebugFIO::Out("s_out.txt") << "attackFrame: " << state.attackFrame << '\n';
					DebugFIO::Out("s_out.txt") << "health: " << state.health << '\n';
					DebugFIO::Out("s_out.txt") << "stunFrame: " << state.stunFrame << '\n';
					DebugFIO::Out("s_out.txt") << "healFrame: " << state.healFrame << '\n';
					DebugFIO::Out("s_out.txt") << "healDelay: " << state.healDelay << '\n';
					DebugFIO::Out("s_out.txt") << "facing: " << state.facing << '\n';
					DebugFIO::Out("s_out.txt") << "spawnPoint: " << state.spawnPoint << '\n';
					DebugFIO::Out("s_out.txt") << "attackFreezeFrame: " << state.attackFreezeFrame << '\n';
					DebugFIO::Out("s_out.txt") << "teamId: " << state.teamId << '\n';
					DebugFIO::Out("s_out.txt") << "stamina: " << state.stamina << '\n';
					DebugFIO::Out("s_out.txt") << "staminaRechargeFrame: " << state.staminaRechargeFrame << '\n';
					DebugFIO::Out("s_out.txt") << "deathFrame: " << state.deathFrame << '\n';
					DebugFIO::Out("s_out.txt") << "attackSpeed: " << state.attackSpeed << '\n';
					DebugFIO::Out("s_out.txt") << "moveSpeed: " << state.moveSpeed << '\n';
					DebugFIO::Out("s_out.txt") << "weaponTag: " << state.weaponTag << '\n';
					DebugFIO::Out("s_out.txt") << "pos: " << state.pos << '\n';
					DebugFIO::Out("s_out.txt") << "vel: " << state.vel << '\n';
					DebugFIO::Out("s_out.txt") << "frozen: " << state.frozen << '\n';
				}
				*/

				onlinePlayers.tickPlayerTimes();

				mode.tickCapturePoints(spawns, CLIENT_TIME_STEP);
				static bool wasRestarting = false;
				if (!wasRestarting && mode.isRestarting()) {
					MessagePacket message{};
					std::string text = "Game over, team " + std::to_string(mode.getWinningTeam()) + " has won.";
					strcpy(message.message, text.data());
					for (auto& pair : users) {
						server.sendPacket(pair.first, message);
					}
				}
				wasRestarting = mode.isRestarting();
			}

			++currentTick;

			std::vector<CapturePointPacket> capturePointPackets;
			auto packetsSize = EntitySystem::GetPool<CapturePointComponent>().size();
			capturePointPackets.reserve(packetsSize);
			for (auto& capturePoint : EntitySystem::GetPool<CapturePointComponent>()) {
				CapturePointPacket packet;
				packet.netId = EntitySystem::GetComp<OnlineComponent>(capturePoint.getId())->getNetId();
				packet.state = capturePoint.getState();
				packet.zone = capturePoint.getZone();
				packet.serialize();
				capturePointPackets.emplace_back(std::move(packet));
			}

			StatePacket state;
			GameState currentState{ ++currGameState };
			if (EntitySystem::Contains<PlayerLC>()) {
				for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
					OnlineComponent* online = EntitySystem::GetComp<OnlineComponent>(player.getId());
					ControllerComponent* controller = EntitySystem::GetComp<ControllerComponent>(player.getId());
					state.state = player.getState();
					state.state.gameTime = gameTime;
					state.when = gameTime;
					state.id = online->getNetId();
					state.controllerState = controller->getController().getState();
					state.prevControllerState = controller->getController().getPrevState();
					currentState.addPlayerState(state);
				}
			}

			for (auto& pair : users) {
				//send the contiguous state block
				//server.sendData(other->getConnection()->getPeer(), 1, states.data(), sizeof(StatePacket) * states.size());
				ServerPlayerComponent& serverPlayer = pair.second->getServerPlayer();
				GameState lastAcknowledged{0};
				MarkedStream m;
				
				if (!serverPlayer.getLastAcknowledged(lastAcknowledged)) {
					m << currentState;
				}
				else {
					currentState.compareAndWrite(m, lastAcknowledged);
				}
				serverPlayer.storeGameState(currentState);
				

				std::vector<char> data;
				data.resize(m.size() + 4);
				std::memcpy(data.data(), "SST", 4);
				std::memcpy(data.data() + 4, m.data(), m.size());
				server.sendData(pair.first, data.data(), data.size());

				GameStateId sentId;
				m >> sentId;
				/*
				DebugFIO::Out("s_out.txt") << "sent gamestate: " << sentId << '\n';
				while (m.hasMoreData()) {
					StatePacket sentState;
					DynamicBitset bits = sentState.readFrom(m);
					DebugFIO::Out("s_out.txt") << "State of player: " << sentState.id << '\n';
					if (bits[PlayerState::b_state])
						DebugFIO::Out("s_out.txt") << "plrState: " << static_cast<unsigned int>(sentState.state.state) << '\n';
					if (bits[PlayerState::b_roll_frame])
						DebugFIO::Out("s_out.txt") << "rollFrame: " << sentState.state.rollFrame << '\n';
					if (bits[PlayerState::b_active_attack])
						DebugFIO::Out("s_out.txt") << "activeAttack: " << sentState.state.activeAttack << '\n';
					if (bits[PlayerState::b_attack_frame])
						DebugFIO::Out("s_out.txt") << "attackFrame: " << sentState.state.attackFrame << '\n';
					if (bits[PlayerState::b_health])
						DebugFIO::Out("s_out.txt") << "health: " << sentState.state.health << '\n';
					if (bits[PlayerState::b_stun_frame])
						DebugFIO::Out("s_out.txt") << "stunFrame: " << sentState.state.stunFrame << '\n';
					if (bits[PlayerState::b_heal_frame])
						DebugFIO::Out("s_out.txt") << "healFrame: " << sentState.state.healFrame << '\n';
					if (bits[PlayerState::b_heal_delay])
						DebugFIO::Out("s_out.txt") << "healDelay: " << sentState.state.healDelay << '\n';
					if (bits[PlayerState::b_facing])
						DebugFIO::Out("s_out.txt") << "facing: " << sentState.state.facing << '\n';
					if (bits[PlayerState::b_spawn_point])
						DebugFIO::Out("s_out.txt") << "spawnPoint: " << sentState.state.spawnPoint << '\n';
					if (bits[PlayerState::b_attack_freeze_frame])
						DebugFIO::Out("s_out.txt") << "attackFreezeFrame: " << sentState.state.attackFreezeFrame << '\n';
					if (bits[PlayerState::b_team_id])
						DebugFIO::Out("s_out.txt") << "teamId: " << sentState.state.teamId << '\n';
					if (bits[PlayerState::b_stamina])
						DebugFIO::Out("s_out.txt") << "stamina: " << sentState.state.stamina << '\n';
					if (bits[PlayerState::b_stamina_recharge_frame])
						DebugFIO::Out("s_out.txt") << "staminaRechargeFrame: " << sentState.state.staminaRechargeFrame << '\n';
					if (bits[PlayerState::b_death_frame])
						DebugFIO::Out("s_out.txt") << "deathFrame: " << sentState.state.deathFrame << '\n';
					if (bits[PlayerState::b_attack_speed])
						DebugFIO::Out("s_out.txt") << "attackSpeed: " << sentState.state.attackSpeed << '\n';
					if (bits[PlayerState::b_move_speed])
						DebugFIO::Out("s_out.txt") << "moveSpeed: " << sentState.state.moveSpeed << '\n';
					if (bits[PlayerState::b_weapon_tag])
						DebugFIO::Out("s_out.txt") << "weaponTag: " << sentState.state.weaponTag << '\n';
					if (bits[PlayerState::b_pos])
						DebugFIO::Out("s_out.txt") << "pos: " << sentState.state.pos << '\n';
					if (bits[PlayerState::b_vel])
						DebugFIO::Out("s_out.txt") << "vel: " << sentState.state.vel << '\n';
					if (bits[PlayerState::b_frozen])
						DebugFIO::Out("s_out.txt") << "frozen: " << sentState.state.frozen << '\n';
				}
				*/

				//DebugFIO::Out("s_out.txt") << "Attempting to send batched player updates.\n";
				server.sendData(pair.first, capturePointPackets.data(), sizeof(CapturePointPacket)* capturePointPackets.size());
			}



			//cleanup after server update, so that all "dead" states can be sent
			EntitySystem::FreeDeadEntities();
			
			size_t size = ctrls.size();
			ctrls.clear();
			ctrls.reserve(size);

			//handle quitting
			QuitPacket q;
			for (auto& pair : users) {
				Connection * con = pair.second->getConnection();
				if (static_cast<double>(currentTick - con->getLastPacket()) * SERVER_TIME_STEP > forceDisconnectDelay) {
					std::cout << "Forcing removal of player " << pair.first << ", " << pair.second->getOnline().getNetId() << ".\n";
					con->setShouldReset(true);
					q.id = pair.second->getOnline().getNetId();
					//tell everyone else
					for (auto& otherPair : users) {
						if(otherPair.first != pair.first)
							server.sendPacket<QuitPacket>(otherPair.first, q);
					}
					server.resetConnection(pair.first);
				}
				else if (static_cast<double>(currentTick - con->getLastPacket()) * SERVER_TIME_STEP > disconnectDelay) {
					std::cout << "Attempting to disconnect player " << pair.first << ", " << pair.second->getOnline().getNetId() << ", no packets received recently.\n";
					server.disconnect(pair.first);
				}
			}

			//remove all resets
			for (auto iter = users.begin(); iter != users.end();) {
				if (iter->second->getConnection()->shouldReset()) {
					mode.removePlayer(iter->second->getId());
					iter->second->deleteUser();
					iter = users.erase(iter);
				}
				else {
					++iter;
				}
			}
		}
	}

	enet_deinitialize();
	SDL_Quit();
	return 0;
}

/*
- create a better sending system, to spread things across multiple channels. packets of each type are required to go allong the same channels
- turn packets into classes. Require serialization and unserialization.
- client side shouldn't set things as truly dead until teh server tells them to. think more about this.

/*
The player state will hold
- Player position
- Player velocity
- time of this state


PlayerLC superclass handles physics
ClientPlayerLC subclass:
- Store the past 32 states
- Re-evaluate the current position based on a change in one of these states.

ServerPlayerLC subclass:
- queue updates
- evaluate queued updates

the client will hold the last 32 player states.
upon recieving a position packet back from the server, it will compare the time of that position packet, to its time.
If its predicted position at a time is different than the servers position, we must recalculate our current position.

A client state packet will have to things:
- Player State
	- Player pos
	- player vel
*/


/*
create a master state class to store all player states, and id
create acknowledgement packet
add master state queue to serverplayercomponent
for each user
	if no states acknowledged or empty
		send full master state
	otherwise
		compare changes, send changes

on receive acknowledgement
	mark state as acknowledged in respective ServerPlayerComponent


when a player joins initialize prev akcnowledged master states to 0
for each client
	send the changes from the last acknowledged master state (if its empty, send the current master state)

on client receive
	send acknowledgement
	update to match all changes
*/