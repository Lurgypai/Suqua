#include "..\header\Game.h"

Game::Game(double physics_step, double render_step) :
	PHYSICS_STEP{physics_step},
	RENDER_STEP{render_step},
	renderTick{0},
	physicsTick{0}
{}

Game::~Game() {}

void Game::prePhysicsStep() {
	for (auto&& scene : scenes) {
		scene->prePhysicsStep(PHYSICS_STEP);
	}
}

void Game::physicsStep() {
	for (auto&& scene : scenes) {
		scene->physicsStep(PHYSICS_STEP);
	}
}

void Game::postPhysicsStep() {
	for (auto&& scene : scenes) {
		scene->postPhysicsStep(PHYSICS_STEP);
	}
}

void Game::preRenderStep() {
	for (auto&& scene : scenes) {
		scene->preRenderStep(render);
	}
}

void Game::renderStep() {
	for (auto&& scene : scenes) {
		scene->renderStep(render);
	}
}

void Game::postRenderStep() {
	for (auto&& scene : scenes) {
		scene->postRenderStep(render);
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
