#include "Game.h"
#include "Packet.h"
#include "PHGameTime.h"
#include "SyncState.h"

Game::Game(double physics_step, double render_step, double server_step, FlagType flags_) :
	PHYSICS_STEP{physics_step},
	RENDER_STEP{render_step},
	SERVER_STEP{server_step},
	renderTick{0},
	physicsTick{0},
	serverTick{0},
	gameTick{0},
	flags{flags_}
{
	if (flags & client) {
		host.createClient(1, 10);
		loadPacketHandler<PHGameTime>(Packet::GameTickId);
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

	uint64_t lastServerUpdate = SDL_GetPerformanceCounter();

	while (true) {

		uint64_t now = SDL_GetPerformanceCounter();
		uint64_t elapsedTime = (now - lastPhysicsUpdate) + leftover;
		lastPhysicsUpdate = now;
		for (; elapsedTime >= physicsDelta; elapsedTime -= physicsDelta) {
			pollSDLEvents();

			if (flags & Flag::input) {
				inputStep();
			}

			if (flags & Flag::physics) {
                physicsUpdate();
			}

			if (flags & (Flag::server | client)) {
				host.handlePackets(*this);
			}

			clearSDLEvents();

			++physicsTick;
		}
		leftover = elapsedTime;

		if (flags & Flag::server) {
			now = SDL_GetPerformanceCounter();
			if (static_cast<double>(now - lastServerUpdate) / SDL_GetPerformanceFrequency() >= SERVER_STEP) {
				ByteStream stream;
				stream << Packet::GameTickId;
				stream << gameTick;
				host.bufferAllData(stream);

                ByteStream statePacket;
                statePacket << Packet::StateId;

                SyncState currState{gameTick};
                currState.serialize(statePacket);
                host.bufferAllData(statePacket);

				host.sendBuffered();
				std::cout << "Packets sent...\n";

				lastServerUpdate = now;
			}
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

    ++gameTick;
}
