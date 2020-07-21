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
#include "MarkedStream.h"

#include "server/User.h"
#include "server/Settings.h"
#include "player/ServerPlayerSystem.h"
#include "player.h"

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
	if (!server.createServer(port, 32, 3)) {
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
	std::list<UserPtr> users;

	PhysicsSystem physics{};
	physics.runPhysics(CLIENT_TIME_STEP);
	CombatSystem combat{};
	WeaponManager weapons{};
	ClimbableSystem climbables{};
	PlayerManager players{};
	ServerPlayerSystem onlinePlayers{};
	DominationMode mode{};
	OnlineSystem online{};

	std::unordered_map<EntityId, PlayerState> prevPlayerStates;
	mode.load(&spawns, 2, 1, 144000);

	for (auto& capturePoint : EntitySystem::GetPool<CapturePointComponent>()) {
		online.addOnlineComponent(capturePoint.getId());
	}

	weapons.loadAttacks("attacks/hit");
	climbables.updateClimbables();

	PeerId clientPeerId = 0;
	DebugFIO::AddFOut("s_out.txt");

	//generate player buffer
	std::vector<EntityId> aiPlayers(aiPlayerCount, 0);
	for (auto& id : aiPlayers) {
		id = players.makePlayer(weapons);
		EntitySystem::MakeComps<AIPlayerComponent>(1, &id);
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

				users.emplace_back(std::make_unique<User>(User{&players, &weapons, clientPeerId, std::make_unique<Connection>(*event.peer, clientPeerId, currentTick) }));
				users.back()->getPlayer().chooseSpawn();
				online.addOnlineComponent(users.back()->getId());

				NetworkId clientNetId = users.back()->getOnline().getNetId();

				mode.addPlayer(users.back()->getId());

				WelcomePacket welcomePacket;
				welcomePacket.currentTick = gameTime;
				welcomePacket.netId = clientNetId;
				server.sendPacket<WelcomePacket>(event.peer, 0, welcomePacket);

				//notify all online players of a new players join
				for (auto& user : users) {
					if (user->getPeerId() != clientPeerId) {
						//tell them about us
						JoinPacket join{};
						join.joinerId = clientNetId;
						server.sendPacket<JoinPacket>(user->getConnection()->getPeer(), 0, join);
					}
				}
				//fill us in about all players

				MarkedStream m;
				for (auto& playerLC : EntitySystem::GetPool<PlayerLC>()) {
					OnlineComponent* online = EntitySystem::GetComp<OnlineComponent>(playerLC.getId());
					if (online->getNetId() != clientNetId) {
						//tell us about them
						JoinPacket us{};
						us.joinerId = online->getNetId();
						server.sendPacket(event.peer, 0, us);
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
					User* sender = nullptr;
					for (auto& user : users) {
						if (user->getPeerId() == senderId)
							sender = user.get();
					}

					if (sender == nullptr)
						std::cout << "Sender was nullptr.\n";
					sender->getConnection()->setLastPacket(currentTick);

					std::string key = PacketUtil::readPacketKey(event.packet);
					if (key == CONT_KEY) {
						ControllerPacket cont{};
						std::memcpy(&cont, event.packet->data, event.packet->dataLength);
						cont.unserialize();

						for (auto& user : users) {
							ServerPlayerComponent& player = user->getServerPlayer();
							if (user->getOnline().getNetId() == cont.netId) {
								ClientCommand comm{ Controller{ cont.state }, cont.clientTime, cont.when };
								player.bufferInput(comm);
							}
						}
					}
					else if (key == TIME_KEY) {
						TimestampPacket time{};
						PacketUtil::readInto<TimestampPacket>(time, event.packet);
						time.unserialize();

						time.gameTime = gameTime;
						time.serverTime = currentTick;
						server.bufferPacket<TimestampPacket>(event.peer, 0, time);
					}
					else if (key == WEAPON_KEY) {
						WeaponChangePacket p{};
						PacketUtil::readInto<WeaponChangePacket>(p, event.packet);
						p.unserialize();
						std::string attackId{};
						attackId.resize(p.size);

						std::memcpy(attackId.data(), (event.packet->data) + sizeof(WeaponChangePacket), p.size);
						bool hasWeapon = weapons.hasWeapon(attackId);
						for (auto& user : users) {
							if (hasWeapon) {
								WeaponChangePacket ret;
								ret.size = attackId.size();
								ret.id = p.id;
								ret.serialize();
								char* data = static_cast<char*>(malloc(sizeof(WeaponChangePacket) + attackId.size()));
								memcpy(data, &ret, sizeof(WeaponChangePacket));
								memcpy(data + sizeof(WeaponChangePacket), attackId.data(), p.size);
								server.sendData(user->getConnection()->getPeer(), 0, data, sizeof(WeaponChangePacket) + attackId.size());
								free(data);

								if (user->getOnline().getNetId() == p.id) {
									user->getCombat().setAttack(attackId);
								}
							}
							else {
								if (user->getOnline().getNetId() == p.id) {
									attackId = user->getCombat().getAttack().getId();
									WeaponChangePacket ret;
									ret.size = attackId.size();
									ret.id = p.id;
									ret.serialize();
									char* data = static_cast<char*>(malloc(sizeof(WeaponChangePacket) + attackId.size()));
									memcpy(data, &ret, sizeof(WeaponChangePacket));
									memcpy(data + sizeof(WeaponChangePacket), attackId.data(), p.size);
									server.sendData(user->getConnection()->getPeer(), 0, data, sizeof(WeaponChangePacket) + attackId.size());
									free(data);
								}
							}
						}
					}
					else if (key == TEAM_KEY) {
						TeamChangePacket p;
						PacketUtil::readInto<TeamChangePacket>(p, event.packet);
						p.unserialize();

						for (auto& user : users) {
							if (user->getOnline().getNetId() == p.id) {
								user->getCombat().teamId = p.targetTeamId;

								server.bufferPacket(event.peer, 0, p);
							}
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

				for (auto& user : users) {
					if (user->getPeerId() == disconnectPeerId) {
						foundUser = true;
						std::cout << "Player " << user->getPeerId() << ", " << user->getOnline().getNetId() << " disconnected.\n";
						q.id = user->getOnline().getNetId();
						user->getConnection()->setShouldReset(true);
					}
				}

				if (!foundUser) {
					throw std::exception{};
				}

				for (auto& user : users) {
					if (user->getPeerId() != disconnectPeerId) {
						server.sendPacket<QuitPacket>(user->getConnection()->getPeer(), 0, q);
					}
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
				for (auto& user : users)
					DebugFIO::Out("s_out.txt") << "Physics to pos: " << user->getPhysics().getPos() << ", vel: " << user->getPhysics().vel << '\n';
				combat.runAttackCheck(CLIENT_TIME_STEP);

				//update all ai players
				if (EntitySystem::Contains<AIPlayerComponent>()) {
					for (auto& aiPlayer : EntitySystem::GetPool<AIPlayerComponent>()) {
						aiPlayer.update();
							players.update(aiPlayer.getId(), lastUpdatedTime, stage, spawns);
					}
				}

				onlinePlayers.updatePlayers(players, CLIENT_TIME_STEP, stage, spawns);
				for (auto& user : users)
					DebugFIO::Out("s_out.txt") << "Updated to pos: " << user->getPhysics().getPos() << ", vel: " << user->getPhysics().vel << '\n';

				for (auto& user : users)
					DebugFIO::Out("s_out.txt") << "Final state at time " << user->getServerPlayer().getClientTime() << ", pos: " << user->getPhysics().getPos() << ", vel: " << user->getPhysics().vel << '\n';

				onlinePlayers.tickPlayerTimes();

				mode.tickCapturePoints(spawns, CLIENT_TIME_STEP);
				static bool wasRestarting = false;
				if (!wasRestarting && mode.isRestarting()) {
					MessagePacket message{};
					std::string text = "Game over, team " + std::to_string(mode.getWinningTeam()) + " has won.";
					strcpy(message.message, text.data());
					for (auto& user : users) {
						server.sendPacket(user->getConnection()->getPeer(), 0, message);
					}
				}
				wasRestarting = mode.isRestarting();
			}

			++currentTick;
			
			MarkedStream m;
			StatePacket state;
			if (EntitySystem::Contains<PlayerLC>()) {
				for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
					OnlineComponent* online = EntitySystem::GetComp<OnlineComponent>(player.getId());
					ControllerComponent* controller = EntitySystem::GetComp<ControllerComponent>(player.getId());
					state.state = player.getState();
					state.when = gameTime;
					state.id = online->getNetId();
					state.controllerState = controller->getController().getState();
					state.serialize();

					const auto& prevState = prevPlayerStates[player.getId()];
					state.readInto(m, prevState);
				}
			}
			
			//legacy state packing
			/*
			std::vector<StatePacket> states;
			states.reserve(EntitySystem::GetPool<PlayerLC>().size());
			for (auto& playerLC : EntitySystem::GetPool<PlayerLC>()) {
				OnlineComponent* online = EntitySystem::GetComp<OnlineComponent>(playerLC.getId());
				ControllerComponent* controller = EntitySystem::GetComp<ControllerComponent>(playerLC.getId());
				StatePacket pos{};
				pos.state = playerLC.getState();
				pos.when = gameTime;
				pos.id = online->getNetId();
				pos.controllerState = controller->getController().getState();

				pos.serialize();
				states.push_back(pos);
				pos.unserialize();
			}
			*/

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

			for (auto& other : users) {
				//send the contiguous state block
				//server.sendData(other->getConnection()->getPeer(), 1, states.data(), sizeof(StatePacket) * states.size());
				std::vector<char> data;
				data.resize(m.size() + 4);
				std::memcpy(data.data(), "SST", 4);
				std::memcpy(data.data() + 4, m.data(), m.size());
				server.sendData(other->getConnection()->getPeer(), 1, data.data(), data.size());
				//DebugFIO::Out("s_out.txt") << "Attempting to send batched player updates.\n";
				server.sendData(other->getConnection()->getPeer(), 2, capturePointPackets.data(), sizeof(CapturePointPacket)* capturePointPackets.size());
			}



			//cleanup after server update, so that all "dead" states can be sent
			EntitySystem::FreeDeadEntities();
			
			size_t size = ctrls.size();
			ctrls.clear();
			ctrls.reserve(size);

			//handle quitting
			QuitPacket q;
			for (auto& user : users) {
				Connection * con = user->getConnection();
				if (static_cast<double>(currentTick - con->getLastPacket()) * SERVER_TIME_STEP > forceDisconnectDelay) {
					std::cout << "Forcing removal of player " << user->getPeerId() << ", " << user->getOnline().getNetId() << ".\n";
					con->setShouldReset(true);
					q.id = user->getOnline().getNetId();
					//tell everyone else
					for (auto& other : users) {
						if(other->getPeerId() != user->getPeerId())
							server.sendPacket<QuitPacket>(other->getPeerId(), 0, q);
					}
					server.resetConnection(user->getPeerId());
				}
				else if (static_cast<double>(currentTick - con->getLastPacket()) * SERVER_TIME_STEP > disconnectDelay) {
					std::cout << "Attempting to disconnect player " << user->getPeerId() << ", " << user->getOnline().getNetId() << ", no packets received recently.\n";
					server.disconnect(user->getPeerId());
				}
			}

			//remove all resets
			auto toReset = [](const UserPtr& user) {
				return user->getConnection()->shouldReset();
			};
			for (auto& user : users) {
				if (toReset(user)) {
					mode.removePlayer(user->getId());
				}
			}
			users.erase(std::remove_if(users.begin(), users.end(), toReset), users.end());
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