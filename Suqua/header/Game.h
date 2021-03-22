#pragma once
#include <SDL.h>
#include <deque>
#include "RenderSystem.h"
#include "Scene.h"
#include "InputDevice.h"
#include "Host.h"
#include "Tick.h"
#include <unordered_map>
#include <vector>

using EventQueue = std::deque<SDL_Event>;

class Game {

friend class SuquaLib;

public:
	using FlagType = char;
	enum Flag : FlagType {
		render = 1 << 0,
		physics = 1 << 1,
		server = 1 << 2,
		client = 1 << 3,
		input =  1 << 4,
		client_flags = render | physics | client | input,
		server_flags = physics | server,
		none = 0
	};

	Game(double physics_step, double render_step, double server_step, FlagType flags_);
	virtual ~Game();

	template<typename S, typename ... Args>
	SceneId loadScene(char flags_, Args... args);

	template<typename T, typename ... Args>
	InputDeviceId loadInputDevice(Args ... args);

	template<typename P, typename ... Args>
	PacketHandlerId loadPacketHandler(PacketHandlerId id, Args... args);

	InputDevice& getInputDevice(InputDeviceId id);

	void setSceneFlags(SceneId id, Scene::FlagType flags_, bool value);
	void toggleSceneFlags(SceneId id, Scene::FlagType flags_);
	void sceneOn(SceneId id);
	void sceneOff(SceneId id);

	void unloadScene(SceneId id);

	const double PHYSICS_STEP;
	const double RENDER_STEP;
	const double SERVER_STEP;

	const RenderSystem& getRender();
	const EventQueue& getEvents();

	void setGameTick(Tick newGameTick);

	Host host;
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
	Tick serverTick;
	Tick gameTick;
	EventQueue events;

	RenderSystem renderSystem;
	std::vector<ScenePtr> scenes;
	std::unordered_map<InputDeviceId, InputDevicePtr> inputDevices;
	FlagType flags;
};

template<typename S, typename ... Args>
inline SceneId Game::loadScene(char flags_, Args... args) {
	ScenePtr scene = std::make_unique<S>(scenes.size(), flags_, args...);
	scene->load(*this);
	scenes.emplace_back(std::move(scene));
	return scenes.size() - 1;
}

template<typename T, typename ...Args>
inline InputDeviceId Game::loadInputDevice(Args ...args) {
	InputDevicePtr inputDevice = std::make_unique<T>(inputDevices.size());
	inputDevices.emplace(inputDevices.size(), std::move(inputDevice));
	return inputDevices.size() - 1;
}

template<typename P, typename ...Args>
inline PacketHandlerId Game::loadPacketHandler(PacketHandlerId id, Args ...args) {
	host.loadPacketHandler<P>(id, args...);
	return id;
}