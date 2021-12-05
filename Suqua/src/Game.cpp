#include "Game.h"
#include "Packet.h"
#include "PHClientPing.h"
#include "PHServerPing.h"
#include "PHSyncState.h"
#include "PHServerInputPacket.h"
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
	serverBroadcastDelay{ serverBroadcastDelay_ }
{
	if (flags & client) {
		host.createClient(1, 10);
		loadPacketHandler<PHClientPing>(Packet::PingId);
		loadPacketHandler<PHSyncState>(Packet::StateId);
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
	Scene* scene;
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
		scene->applyInputs(*this);
	}
}
}

void Game::prePhysicsStep() {
	for (auto&& scene : scenes) {
		if (scene->flags & Scene::Flag::physics) {
			scene->prePhysicsStep(*this);
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

void Game::postPhysicsStep() {
	for (auto&& scene : scenes) {
		if (scene->flags & Scene::Flag::physics) {
			scene->postPhysicsStep(*this);
		}
	}
}

void Game::preRenderStep() {
	for (auto&& scene : scenes) {
		if (scene->flags & Scene::Flag::physics) {
			scene->preRenderStep(*this);
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

void Game::postRenderStep() {
	for (auto&& scene : scenes) {
		if (scene->flags & Scene::Flag::physics) {
			scene->postRenderStep(*this);
		}
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

void Game::loop() {

	uint64_t lastPhysicsUpdate = SDL_GetPerformanceCounter();
	uint64_t leftover = 0;
	uint64_t physicsDelta = PHYSICS_STEP * SDL_GetPerformanceFrequency();

	uint64_t lastGFXUpdate = SDL_GetPerformanceCounter();
	uint64_t now;

	while (true) {
		//if we're a server, run up until the latest client input is received
		if (flags & Flag::server) {
			pollSDLEvents();

			if (serverInputQueue.allReceived(host, gameTick)) {
				//std::cout << "Received inputs for tick " << gameTick << '\n';
				serverInputQueue.applyInputs(online, gameTick);
				physicsUpdate();

				host.handlePackets(*this);

				host.sendBuffered();
				sync.storeCurrentState(gameTick);

				++serverBroadcastCtr;
				if (serverBroadcastCtr == serverBroadcastDelay - 1) {
					serverBroadcastCtr = 0;

					ByteStream statePacket;
					sync.writeStatePacket(statePacket, gameTick);
					//std::cout << "Buffering for broarcast for tick " << gameTick << '\n';
					host.bufferAllDataByChannel(0, statePacket);
				}

				tickTime();
				clearSDLEvents();
			}
			else {
				//std::cout << "Have not received inputs for tick " << gameTick << '\n';
				host.handlePackets(*this);
				host.sendBuffered();
			}
		}

		//if we're not a server, operate like client or non-online client
		else {
			now = SDL_GetPerformanceCounter();
			uint64_t elapsedTime = (now - lastPhysicsUpdate) + leftover;
			lastPhysicsUpdate = now;
			for (; elapsedTime >= physicsDelta; elapsedTime -= physicsDelta) {
				pollSDLEvents();

				if (flags & Flag::input) {
					inputStep();

					if (flags & Flag::client) {
						for (NetworkId ownedNetId : ownedNetIds) {
							ByteStream inputPacket;
							inputPacket << Packet::InputId;
							inputPacket << gameTick;
							inputPacket << ownedNetId;
							Controller& cont = EntitySystem::GetComp<ControllerComponent>(online.getEntity(ownedNetId))->getController();
							cont.serialize(inputPacket);
							host.bufferAllDataByChannel(1, inputPacket);
						}
					}
				}

				if (flags & Flag::physics) {
					physicsUpdate();
					//std::cout << "currentTick: " << gameTick << '\n';
					if (flags & Flag::client) sync.interpolate(gameTick - (2 * serverBroadcastDelay));
				}


				if (flags & Flag::client) {
					host.handlePackets(*this);
					host.sendBuffered();
					sync.storeCurrentState(gameTick);
				}

				//if (flags & Flag::client) {
				//	if (clientPingCtr < clientPingDelay) ++clientPingCtr;
				//	else {
				//		clientPingCtr = 0;
				//		ByteStream pingPacket;
				//		pingPacket << Packet::PingId;
				//		pingPacket << gameTick;
				//		host.bufferAllDataByChannel(0, pingPacket);
				//		std::cout << "Client pinging server...\n";
				//	}
				//}

				clearSDLEvents();

				tickTime();
			}
			leftover = elapsedTime;
		}

		if (flags & Flag::render) {
			now = SDL_GetPerformanceCounter();
			if (static_cast<double>(now - lastGFXUpdate) / SDL_GetPerformanceFrequency() >= RENDER_STEP) {
				preRenderStep();
				renderStep();
				postRenderStep();
				++renderTick;

				lastGFXUpdate = now;
			}
		}

		EntitySystem::FreeDeadEntities();
	}
}

void Game::onConnect(PeerId id) {
	for (auto& scenePtr : scenes) {
		scenePtr->onConnect(*this, id);
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
    prePhysicsStep();
    physicsStep();
    postPhysicsStep();
}
