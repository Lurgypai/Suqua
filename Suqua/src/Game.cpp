#include "Game.h"
#include "Packet.h"
#include "PHClientPing.h"
#include "PHServerPing.h"
#include "PHSyncState.h"
#include "PHServerInputPacket.h"
#include "PHOOSPacket.h"
#include "SyncState.h"

Game::Game(FlagType flags_, double physics_step, double render_step, Tick clientPingDelay_, Tick serverBroadcastDelay_) :
	PHYSICS_STEP{ physics_step },
	RENDER_STEP{ render_step },
	TICK_RATE{ 1.0 / PHYSICS_STEP },
	renderTick{ 0 },
	gameTick{ 0 },
	flags{ flags_ },
	clientPingCtr{ 0 },
	clientPingDelay{ clientPingDelay_ },
	serverBroadcastCtr{ 0 },
	serverBroadcastDelay{ serverBroadcastDelay_ },
	networkInputDelay{ 0 },
	networkInputTimeout{ 0 }
{
	if (flags & client) {
		host.createClient(1, 10);
		loadPacketHandler<PHClientPing>(Packet::PingId);
		loadPacketHandler<PHSyncState>(Packet::StateId);
		loadPacketHandler<PHOOSPacket>(Packet::OOSId);
	}
	if (flags & server) {
		host.createServer(25565, 10, 10);
		loadPacketHandler<PHServerPing>(Packet::PingId);
		loadPacketHandler<PHServerInputPacket>(Packet::InputId);
	}
}

Game::~Game() {}

InputDevice& Game::getInputDevice(InputDeviceId id) {
	return *inputDevices.at(id);
}

void Game::setSceneFlags(SceneId id, Scene::FlagType flags_, bool value) {
	Scene* scene = nullptr;
	for (auto&& s : scenes) {
		if (s->getId() == id)
			scene = s.get();
	}
	//scene not found
	if (!scene)
		throw std::exception{};

	if (!value)
		scene->flags &= ~flags_;
	else
		scene->flags |= flags_;
}

void Game::toggleSceneFlags(SceneId id, Scene::FlagType flags_) {
	Scene* scene;
	for (auto&& s : scenes) {
		if (s->getId() == id)
			scene = s.get();
	}
	//scene not found
	if (!scene)
		throw std::exception{};

	scene->flags ^= flags_;
}

void Game::sceneOn(SceneId id) {
	Scene* scene;
	for (auto&& s : scenes) {
		if (s->getId() == id)
			scene = s.get();
	}

	//scene not found
	if (!scene)
		throw std::exception{};

	scene->flags = Scene::all;
}

void Game::sceneOff(SceneId id) {
	Scene* scene;
	for (auto&& s : scenes) {
		if (s->getId() == id)
			scene = s.get();
	}

	//scene not found
	if (!scene)
		throw std::exception{};

	scene->flags = Scene::none;
}

void Game::unloadScene(SceneId id) {
	Scene* scene;
	for (auto&& s : scenes) {
		if (s->getId() == id)
			scene = s.get();
	}

	//scene not found
	if (!scene)
		throw std::exception{};

    scene->unload(*this);
    scene->removeAllEntities();
    for(auto sceneIter = scenes.begin(); sceneIter != scenes.end(); ++sceneIter) {
        if((*sceneIter)->getId() == id) {
            scenes.erase(sceneIter);
            break;
        }
    }
}

void Game::close() {
    for(auto sceneIter = scenes.begin(); sceneIter != scenes.end();) {
        (*sceneIter)->unload(*this);
        (*sceneIter)->removeAllEntities();
        sceneIter = scenes.erase(sceneIter);
    }
}

void Game::tickTime() {
	++gameTick;
}

void Game::inputStep() {
	for (auto&& scene : scenes) {
		if (scene->flags & Scene::Flag::input) {
			scene->storeInputs(*this);
			scene->applyInputs(*this);
		}
	}
}

void Game::physicsStep() {
	for (auto&& scene : scenes) {
		if (scene->flags & Scene::Flag::physics) {
			scene->physicsStep(*this);
		}
	}
}

void Game::renderUpdateStep() {
	for (auto&& scene : scenes) {
		if (scene->flags & Scene::Flag::physics) {
			scene->renderUpdateStep(*this);
		}
	}
}

void Game::renderStep() {
	for (auto&& scene : scenes) {
		if (scene->flags & Scene::Flag::physics) {
			scene->renderStep(*this);
		}
	}
}

void Game::cleanScenes() {
	for (auto&& scene : scenes) {
		scene->removeDeadEntities();
	}
}

Game::FlagType Game::getFlags() {
	return flags;
}

void Game::pollSDLEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		events.emplace_back(std::move(e));
	}
}

void Game::clearSDLEvents() {
	events.clear();
}

void Game::serverStep() {
	serverInputQueue.applyInputs(online, gameTick);
	physicsUpdate();

	host.handlePackets(*this);

	host.sendBuffered();
	sync.storeCurrentState(gameTick);

	if (serverBroadcastCtr == serverBroadcastDelay) {
		serverBroadcastCtr = 0;

		ByteStream statePacket;
		sync.writeStatePacket(statePacket, gameTick);
		//std::cout << "Buffering for broadcast for tick " << gameTick << '\n';
		host.bufferAllDataByChannel(0, statePacket);
	}
	else {
		++serverBroadcastCtr;
	}

	tickTime();
	clearSDLEvents();
}

void Game::loop() {

	uint64_t lastPhysicsUpdate = SDL_GetPerformanceCounter();
	uint64_t lastNetworkUpdate = SDL_GetPerformanceCounter();
	uint64_t leftover = 0;
	uint64_t physicsDelta = PHYSICS_STEP * SDL_GetPerformanceFrequency();
	// uint64_t networkDelta = networkInputTimeout * SDL_GetPerformanceFrequency();

	uint64_t lastGFXUpdate = SDL_GetPerformanceCounter();
	uint64_t now;

	while (true) {
		//if we're a server, run up until the latest client input is received
		if (flags & Flag::server) {


			now = SDL_GetPerformanceCounter();
			uint64_t elapsedTime = (now - lastNetworkUpdate) + leftover;

			pollSDLEvents();
			
			lastNetworkUpdate = now;
			for (; elapsedTime >= physicsDelta; elapsedTime -= physicsDelta) {
				serverStep();
				
			}
			leftover = elapsedTime;

			/*
			if (serverInputQueue.allReceived(host, gameTick)) {
				lastNetworkUpdate = now;
				
				// std::cout << "Received inputs for tick " << gameTick << '\n';
				serverStep();
			}
			*/

			// add a toggle for prediction
			// test just delayed netcode

			// this might not be the issue!
			// it looks like there might be a clogging issue
			/*
			* When another player dcs,
			* this hits the maximum over and over until the server realized they've dc'd
			* this in turn causes the server to slow down significantly
			* when the client then pings the server,
			* they realize they're ahead
			* so they jump backwards
			* causing them to repeatedly jump backwards.
			*/

			/*
			else {
				// if we hit the delay, run the update anyway.
				if (networkDelta != 0 && elapsedTime >= networkDelta) {
					lastNetworkUpdate = now;

					std::cout << "Server waited " << networkInputTimeout << " second(s) and was still missing inputs for time " << gameTick << ", skipping missed inputs.\n";

					serverStep();
				}
				else {
					//std::cout << "Have not received inputs for tick " << gameTick << '\n';
					host.handlePackets(*this);
					host.sendBuffered();
				}
			}
			*/
		}

		//if we're not a server, operate like client or non-online client
		else {
			now = SDL_GetPerformanceCounter();
			uint64_t elapsedTime = (now - lastPhysicsUpdate) + leftover;
			lastPhysicsUpdate = now;
			for (; elapsedTime >= physicsDelta; elapsedTime -= physicsDelta) {
				pollSDLEvents();

				/*
				* INPUT CODE
				* inputStep():
				*	Poll inputs.
				*   Store them for the correct time in the future.
				*	Apply current inputs
				* 
				* Send current inputs to server
				*/

				if (flags & Flag::input) {
					inputStep();

					if (flags & Flag::client) {

						// send all current inputs
						for (auto&& scene : scenes) {
							if (scene->flags & Scene::Flag::input) {
								const auto* currInputs = scene->getInputsAtTime(gameTick + networkInputDelay);
								if (currInputs) {
									for (NetworkId ownedNetId : ownedNetIds) {
										ByteStream inputPacket;
										inputPacket << Packet::InputId;
										inputPacket << gameTick + networkInputDelay;
										inputPacket << ownedNetId;

										//don't like these copies, may be an issue
										Controller cont = currInputs->find(online.getEntity(ownedNetId))->second;
										cont.serialize(inputPacket);
										std::cout << "Sent inputs for tick " << gameTick + networkInputDelay << " on tick " << gameTick << ".\n";
										host.bufferAllDataByChannel(1, inputPacket);
									}
								}
							}
						}
					}
				}

				if (flags & Flag::physics) {
					physicsUpdate();
					renderUpdateStep();
					//std::cout << "currentTick: " << gameTick << '\n';
					//if (flags & Flag::client) sync.interpolate(gameTick - (2 * serverBroadcastDelay));
				}


				if (flags & Flag::client) {
					host.handlePackets(*this);
					host.sendBuffered();
					sync.storeCurrentState(gameTick);
				}

				if (flags & Flag::client) {
					if (clientPingCtr < clientPingDelay) ++clientPingCtr;
					else {
						clientPingCtr = 0;
						ByteStream pingPacket;
						pingPacket << Packet::PingId;
						pingPacket << gameTick;
						host.bufferAllDataByChannel(0, pingPacket);
						// std::cout << "Client pinging server...\n";
					}
				}

				clearSDLEvents();

				tickTime();
			}
			leftover = elapsedTime;
		}

		if (flags & Flag::render) {
			now = SDL_GetPerformanceCounter();
			if (static_cast<double>(now - lastGFXUpdate) / SDL_GetPerformanceFrequency() >= RENDER_STEP) {
				GLRenderer::Clear();
				renderStep();
				GLRenderer::Swap();
				++renderTick;

				lastGFXUpdate = now;
			}
		}

		cleanScenes();
		EntitySystem::FreeDeadEntities();
	}
}

void Game::onConnect(PeerId id) {
	for (auto& scenePtr : scenes) {
		scenePtr->onConnect(*this, id);
	}
}

void Game::onDisconnect(PeerId id) {
	for (auto& scenePtr : scenes) {
		scenePtr->onDisconnect(*this, id);
	}
}

void Game::addOwnedNetId(NetworkId id) {
	ownedNetIds.emplace_back(id);
}

void Game::removeOwnedNetId(NetworkId id) {
	ownedNetIds.erase(std::remove(ownedNetIds.begin(), ownedNetIds.end(), id), ownedNetIds.end());
}

const std::vector<NetworkId>& Game::getOwnedNetIds() const {
	return ownedNetIds;
}

const RenderSystem& Game::getRender() {
	return renderSystem;
}

const EventQueue& Game::getEvents() {
	return events;
}

void Game::setGameTick(Tick newGameTick) {
	gameTick = newGameTick;
}

Tick Game::getGameTick() const {
    return gameTick;
}

void Game::physicsUpdate() {
    physicsStep();
}