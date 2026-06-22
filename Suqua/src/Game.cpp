#include "Game.h"
#include "Packet.h"
#include "SuquaLib.h"
#include "DebugIO.h"
#include "NetworkDataComponent.h"

Game::Game(FlagType flags_, double physics_step, double render_step) :
    // timing
	PHYSICS_STEP{ physics_step },
	RENDER_STEP{ render_step },
	TICK_RATE{ 1.0 / PHYSICS_STEP },
    // current ticks
	renderTick{ 0 },
	gameTick{ 0 },
    // update flags
	flags{ flags_ },
    //networking timers
	networkInputTimeout{ 0 },
    stateBroadcastDelay{ 1 },
    stateBroadcastDelayCtr{ 0 } 
{
	if (flags & client) {
		host.createClient(1, 10);
	}
	if (flags & server) {
		host.createServer(25565, 10, 10);
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
		if (scene->flags & Scene::Flag::render) {
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

void Game::broadcastOwnedStates() {
    // check if update time
    ++stateBroadcastDelayCtr;
    if(stateBroadcastDelayCtr != stateBroadcastDelay) return;
    stateBroadcastDelayCtr = 0;

    //update
    ByteStream state;
    state << Packet::StateId;
    state << true;
    if(!EntitySystem::Contains<NetworkDataComponent>()) return;

    for (auto& ndc : EntitySystem::GetPool<NetworkDataComponent>()) {
        if (ndc.owner != NetworkDataComponent::Owner::local_shared) continue;

        ndc.serializeForNetwork(state);
    }
    host.bufferAllDataByChannel(0, state);
}

void Game::broadcastDeadEntities() {
    for(auto& scene : scenes) {
        scene->broadcastDeadEntities(*this);
    }
};

void Game::serverStep() {
	if (flags & Flag::input) {
		tickInputDevices();
		inputStep();
	}

    if(flags & Flag::physics) {
        physicsUpdate();
    }

    broadcastOwnedStates();
	host.handlePackets(*this);
    broadcastDeadEntities();
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
            else if (e.key.keysym.sym == SDLK_SLASH) {
                if(!DebugIO::getOpen()) {
                    DebugIO::openDebug();
                    DebugIO::addInput("/");
                }
            }
            else if (e.key.keysym.sym == SDLK_BACKSPACE)
				DebugIO::backspace();
            else if (e.key.keysym.sym == SDLK_RETURN)
				DebugIO::enterInput();
			break;
		}
	}

	if (flags & Flag::input) {
		if (!DebugIO::getOpen()) {
            tickInputDevices();
            inputStep();
        }
	}

	if (flags & Flag::physics) {
		physicsUpdate();
		renderUpdateStep();
	}

	if (flags & Flag::client) {
        // broadcast our deltas first. this updates the "previous" values used to track deltas
        broadcastOwnedStates();
        // handle incoming packets.
        // This also updates the previous values to prevent new deltas from being stored
		host.handlePackets(*this);
        // Now that we've applied possible death from the server, broadcast anything that has died
        broadcastDeadEntities();
        // send the buffered (including death) packets
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
            tickTime();
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


		cleanScenes();
		EntitySystem::FreeDeadEntities();

        host.updateDelayed();
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

void Game::setStateBroadcastDelay(Tick delay) {
    if(delay > 0) stateBroadcastDelay = delay;
    else throw std::runtime_error{"stateBroadcastDelay cannot be less than 0.\n"};
}
