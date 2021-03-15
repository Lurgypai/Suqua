#include "..\header\Game.h"

Game::Game(double physics_step, double render_step) :
	PHYSICS_STEP{physics_step},
	RENDER_STEP{render_step},
	renderTick{0},
	physicsTick{0}
{}

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

	while (true) {
		uint64_t now = SDL_GetPerformanceCounter();
		uint64_t elapsedTime = (now - lastPhysicsUpdate) + leftover;
		lastPhysicsUpdate = now;
		for (; elapsedTime >= physicsDelta; elapsedTime -= physicsDelta) {
			pollSDLEvents();

			inputStep();

			prePhysicsStep();
			physicsStep();
			postPhysicsStep();

			clearSDLEvents();

			++physicsTick;
		}
		leftover = elapsedTime;

		now = SDL_GetPerformanceCounter();
		if (static_cast<double>(now - lastGFXUpdate) >= RENDER_STEP) {
			preRenderStep();
			renderStep();
			postRenderStep();
			++renderTick;

			lastGFXUpdate = now;
		}


		EntitySystem::FreeDeadEntities();
	}
}

const RenderSystem& Game::getRender() {
	return render;
}

const EventQueue& Game::getEvents() {
	return events;
}
