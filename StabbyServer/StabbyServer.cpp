// StabbyServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "enet/enet.h"

#include "SDL.h"
#include "NetworkTypes.h"
#include "Packet.h"
#include "Controller.h"
#include <list>
#include <algorithm>
#include <vector>

#include "Connection.h"
#include "User.h"
#include "Stage.h"

#include "ServerClientData.h"

#define MAX_PACKET_COUNT 500
#define CLIENT_SIDE_DELTA 1.0 / 120

//I don't know how to copy peers, or If I should even try. Thus, to simplifiy things copying of connections is not allowed,
//to ensure peer->data isn't deleted multiple times

using UserPtr = std::unique_ptr<User>;

int main(int argv, char* argc[])
{

	SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER);
	std::cout << "Starting server...\n";
	enet_initialize();
	ENetAddress address;
	ENetHost * server;

	address.host = ENET_HOST_ANY;
	address.port = 25565;

	server = enet_host_create(&address, 32, 2, 0, 0);

	if (server == NULL) {
		std::cout << "Oops, no server! Perhaps one is already running on this port?\n";
		return -1;
	}
	std::cout << "Beginning server loop.\n";


	//wait this long to disconnect a client
	double disconnectDelay{ 5.0 };
	//force a disconenct at this point
	double forceDisconnectDelay{ 10.0 };

	Uint64 prev = SDL_GetPerformanceCounter();

	ENetPacket * packet;
	WelcomePacket welcomePacket;

	//current tick in server time
	Time_t currentTick{0};
	double serverDelta{0};

	Time_t gameTime{ 0 };
	NetworkId incrementer{ 0 };

	std::vector<ControllerPacket> ctrls;
	std::list<UserPtr> users;

	Stage stage{};

	//switch game to tick at client speed, but only send updates out at server speed
	//test this, it looks like its done.

	while (true) {

		Time_t now = SDL_GetPerformanceCounter();
		if (static_cast<double>(now - prev) / SDL_GetPerformanceFrequency() >= CLIENT_TIME_STEP) {
			gameTime += CLIENT_TIME_STEP / GAME_TIME_STEP;
			prev = now;

			//std::cout << "Server tick, polling packets...\n";
			ENetEvent event;
			int packetsPolled = 0;
			while (enet_host_service(server, &event, 0) > 0) {
				switch (event.type) {
				case ENET_EVENT_TYPE_CONNECT:
					std::cout << "Connection received.\n";
					welcomePacket.currentTick = gameTime;
					welcomePacket.netId = ++incrementer;
					packet = enet_packet_create(&welcomePacket, sizeof(welcomePacket), ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(event.peer, 0, packet);


					//notify all online players of a new players join
					for (auto& user : users) {
						JoinPacket join{};
						join.joinerId = incrementer;
						enet_peer_send(user->getConnection()->getPeer(), 0, enet_packet_create(&join, sizeof(join), ENET_PACKET_FLAG_RELIABLE));

						JoinPacket us{};
						us.joinerId = user->getNetId();
						enet_peer_send(event.peer, 0, enet_packet_create(&us, sizeof(us), ENET_PACKET_FLAG_RELIABLE));
					}

					users.emplace_back(std::make_unique<User>(User{incrementer, std::make_unique<Connection>(*event.peer, incrementer, currentTick) }));
					users.back()->getPlayer().setWhen(gameTime);
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					if (event.packet->dataLength == 0) {
						std::cout << "Garbage packet received.\n";
					}
					else {
						NetworkId senderId = *static_cast<NetworkId *>(event.peer->data);
						User * sender = nullptr;
						for (auto& user : users) {
							if (user->getNetId() == senderId)
								sender = user.get();
						}

						if (sender == nullptr)
							std::cout << "Sender was nullptr.\n";
						sender->getConnection()->setLastPacket(currentTick);

						std::string key = PacketUtil::readPacketKey(event.packet);
						if (key == CONT_KEY) {
							ControllerPacket cont{};
							std::memcpy(&cont, event.packet->data, event.packet->dataLength);

							for (auto& user : users) {
								ServerPlayerLC& player = user->getPlayer();
								if (user->getNetId() == cont.netId) {
									ClientCommand comm{ Controller{ cont.state }, cont.time };
									player.bufferInput(comm);
								}
							}
						}
						else if (key == TIME_KEY) {
							TimestampPacket time{};
							PacketUtil::readInto<TimestampPacket>(time, event.packet);
							time.gameTime = gameTime;
							time.serverTime = currentTick;
							enet_peer_send(event.peer, 0, enet_packet_create(&time, sizeof(TimestampPacket), 0));
						}
						
					}
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					{
						QuitPacket q;
						NetworkId peerNetId;
						std::memcpy(&peerNetId, event.peer->data, sizeof(peerNetId));
						q.id = peerNetId;
						std::cout << "Player " << peerNetId << " disconnected.\n";
						for (auto iter = users.begin(); iter != users.end(); ++iter) {
							auto& user = *iter;
							auto con = user->getConnection();
							if (user->getNetId() == peerNetId) {
								con->setShouldReset(true);
							}
							else {
								enet_peer_send(con->getPeer(), 0, enet_packet_create(&q, sizeof(q), 0));
							}
						}
					}
					break;
				default:
					std::cout << "Event handled.\n";
				}
				++packetsPolled;
			}
			
			//move
			for (auto& user : users) {
				user->getPlayer().update(gameTime);
			}

			//after movement, run collisisons
			for (auto& user : users) {
				user->getPlayer().runHitDetect(gameTime);
			}

			serverDelta += CLIENT_TIME_STEP / SERVER_TIME_STEP;

			if (serverDelta >= 1) {
				currentTick += round(serverDelta);
				serverDelta = 0;
				//tell all
				for (auto& user : users) {
					for (auto& other : users) {

						ServerPlayerLC & player = user->getPlayer();
						StatePacket pos{};
						pos.state.pos = player.getPos();
						pos.state.vel = player.getVel();
						pos.state.when = player.getWhen();
						pos.state.activeAttack = player.getAttack().getActiveId();
						pos.state.attackFrame = player.getAttack().getCurrFrame();
						pos.state.rollFrame = player.getRollFrame();
						pos.state.health = player.getHealth();
						pos.state.stunFrames = player.getStunFrame();
						pos.state.state = player.getState();
						pos.when = gameTime;
						pos.id = user->getNetId();

						enet_peer_send(other->getConnection()->getPeer(), 0, enet_packet_create(&pos, sizeof(pos), 0));
					}
				}
			}

			
			size_t size = ctrls.size();
			ctrls.clear();
			ctrls.reserve(size);

			//handle quitting
			QuitPacket q;
			for (auto& user : users) {
				Connection * con = user->getConnection();
				if (static_cast<double>(currentTick - con->getLastPacket()) * SERVER_TIME_STEP > forceDisconnectDelay) {
					std::cout << "Forcing removal of player " << user->getNetId() << ".\n";
					con->setShouldReset(true);
					q.id = user->getNetId();
					//tell everyone else
					for (auto& other : users) {
						if(other->getNetId() != user->getNetId())
							enet_peer_send(other->getConnection()->getPeer(), 0, enet_packet_create(&q, sizeof(q), 0));
					}
					enet_peer_reset(con->getPeer());
				}
				else if (static_cast<double>(currentTick - con->getLastPacket()) * SERVER_TIME_STEP > disconnectDelay) {
					std::cout << "Attempting to disconnect player " << user->getNetId() << ", no packets received recently.\n";
					enet_peer_disconnect(con->getPeer(), 0);
				}
			}

			//remove all resets
			auto toReset = [](const UserPtr& user) {
				return user->getConnection()->shouldReset();
			};
			users.erase(std::remove_if(users.begin(), users.end(), toReset), users.end());
		}
	}

	enet_host_destroy(server);
	enet_deinitialize();
	SDL_Quit();
	return 0;
}
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