#include "Game.h"
#include "Packet.h"
#include "PHClientPing.h"
#include "PHServerPing.h"
#include "PHSyncState.h"
#include "PHServerInputPacket.h"
#include "PHOOSPacket.h"
#include "SyncState.h"
#include "SuquaLib.h"
#include "DebugIO.h"
#include "NetworkOwnerComponent.h"
#include "OnlineComponent.h"

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
			scene->doInputs(*this);
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
	physicsUpdate();
	if (serverBroadcastCtr == serverBroadcastDelay) {
		serverBroadcastCtr = 0;

		ByteStream statePacket;
		statePacket << Packet::StateId;
		for (auto& ndc : EntitySystem::GetPool<NetworkDataComponent>()) {
			auto onlineComp = EntitySystem::GetComp<OnlineComponent>(ndc.getId());
			statePacket << onlineComp->getNetId();
			ndc.serializeForNetwork(statePacket);
		}
		host.bufferAllDataByChannel(0, statePacket);
	}
	else {
		++serverBroadcastCtr;
	}

	host.handlePackets(*this);
	host.sendBuffered();
}

void Game::clientStep() {
	for (auto& e : events) {
		switch (e.type) {
		case SDL_TEXTINPUT:
			if (DebugIO::getOpen())
				DebugIO::addInput(e.text.text);
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_BACKQUOTE)
				DebugIO::toggleDebug();
			else if (e.key.keysym.sym == SDLK_BACKSPACE)
				DebugIO::backspace();
			else if (e.key.keysym.sym == SDLK_RETURN)
				DebugIO::enterInput();
			break;
		}
	}

	if (flags & Flag::input) {
		tickInputDevices();
		if (!DebugIO::getOpen()) inputStep();
	}

	if (flags & Flag::physics) {
		physicsUpdate();
		renderUpdateStep();
	}


	if (flags & Flag::client) {
		// send governed entity states
		if (EntitySystem::Contains<OnlineComponent>()) {
			ByteStream state;
			state << Packet::StateId;
			for (auto& networkOwnerComp : EntitySystem::GetPool<NetworkOwnerComponent>()) {
				if (networkOwnerComp.owner == NetworkOwnerComponent::Owner::local) {
					auto onlineComp = EntitySystem::GetComp<OnlineComponent>(networkOwnerComp.getId());
					if (onlineComp == nullptr) continue;
					auto ndc = EntitySystem::GetComp<NetworkDataComponent>(networkOwnerComp.getId());
					state << onlineComp->getNetId();
					ndc->serializeForNetwork(state);
				}
			}
		}

		host.handlePackets(*this);
		host.sendBuffered();
	}
}

void Game::tickInputDevices() {
	for (auto& [id, inputDevice] : inputDevices) {
		inputDevice->update();
	}
}

void Game::loop() {

	uint64_t lastPhysicsUpdate = SDL_GetPerformanceCounter();
	uint64_t leftover = 0;
	uint64_t physicsDelta = PHYSICS_STEP * SDL_GetPerformanceFrequency();

	uint64_t lastGFXUpdate = SDL_GetPerformanceCounter();
	uint64_t now;

	while (true) {
		now = SDL_GetPerformanceCounter();
		uint64_t elapsedTime = (now - lastPhysicsUpdate) + leftover;

		pollSDLEvents();

		lastPhysicsUpdate = now;
		for (; elapsedTime >= physicsDelta; elapsedTime -= physicsDelta) {
			if (flags & Flag::server) serverStep();
			else clientStep();

			clearSDLEvents();
		}
		leftover = elapsedTime;

		if (flags & Flag::render) {
			now = SDL_GetPerformanceCounter();
			if (static_cast<double>(now - lastGFXUpdate) / SDL_GetPerformanceFrequency() >= RENDER_STEP) {
				GLRenderer::Clear();
				renderStep();
				SuquaLib::DrawConsole();
				GLRenderer::Swap();
				++renderTick;

				lastGFXUpdate = now;
			}
		}

		tickTime();

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