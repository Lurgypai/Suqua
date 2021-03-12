#pragma once
#include <SDL.h>
#include <deque>
#include "RenderSystem.h"
#include "Scene.h"
#include <vector>

using EventQueue = std::deque<SDL_Event>;

class Game {

friend class SuquaLib;

public:
	using Tick = unsigned long long;

	Game(double physics_step, double render_step);
	virtual ~Game();

	template<typename S>
	void loadScene();

	const double PHYSICS_STEP;
	const double RENDER_STEP;

private:
	void pollSDLEvents();
	void clearSDLEvents();
	void loop();

	void prePhysicsStep();
	void physicsStep();
	void postPhysicsStep();

	void preRenderStep();
	void renderStep();
	void postRenderStep();

	Tick renderTick;
	Tick physicsTick;
	EventQueue events;

	RenderSystem render;
	std::vector<ScenePtr> scenes;
};

template<typename S>
inline void Game::loadScene() {
	ScenePtr scene = std::make_unique<S>(scenes.size());
	scene->load();
	scenes.emplace_back(std::move(scene));
}
