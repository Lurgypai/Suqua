#pragma once
#include <SDL.h>
#include <deque>
#include "RenderSystem.h"
#include "Scene.h"
#include "InputDevice.h"
#include <unordered_map>
#include <vector>

using EventQueue = std::deque<SDL_Event>;

class Game {

friend class SuquaLib;

public:
	using Tick = unsigned long long;

	Game(double physics_step, double render_step);
	virtual ~Game();

	template<typename S, typename ... Args>
	SceneId loadScene(char flags_, Args... args);

	template<typename T, typename ... Args>
	InputDeviceId loadInputDevice(Args ... args);

	InputDevice& getInputDevice(InputDeviceId id);

	void setSceneFlags(SceneId id, Scene::FlagType flags_, bool value);
	void toggleSceneFlags(SceneId id, Scene::FlagType flags_);
	void sceneOn(SceneId id);
	void sceneOff(SceneId id);

	void unloadScene(SceneId id);

	const double PHYSICS_STEP;
	const double RENDER_STEP;

	const RenderSystem& getRender();
	const EventQueue& getEvents();

private:
	void pollSDLEvents();
	void clearSDLEvents();
	void loop();

	void inputStep();

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
	std::unordered_map<InputDeviceId, InputDevicePtr> inputDevices;
};

template<typename S, typename ... Args>
inline SceneId Game::loadScene(char flags_, Args... args) {
	ScenePtr scene = std::make_unique<S>(scenes.size(), flags_, args...);
	scene->load();
	scenes.emplace_back(std::move(scene));
	return scenes.size() - 1;
}

template<typename T, typename ...Args>
inline InputDeviceId Game::loadInputDevice(Args ...args) {
	InputDevicePtr inputDevice = std::make_unique<T>(inputDevices.size());
	inputDevices.emplace(inputDevices.size(), std::move(inputDevice));
	return inputDevices.size() - 1;
}