#pragma once
#include "SDL.h"
#include <deque>
#include "RenderSystem.h"
#include "Scene.h"
#include "InputDevice.h"
#include "Host.h"
#include "Tick.h"
#include "OnlineSystem.h"
#include "SyncSystem.h"
#include "ServerInputQueue.h"

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

	Game(FlagType flags, double physics_step = (1.0 / 120.0), double render_step = (1.0 / 60.0), Tick clientPingDelay_ = 120, Tick serverBroadcastDelay_ = 4);
	virtual ~Game();

	template<typename S, typename ... Args>
	SceneId loadScene(char flags_, Args... args);

	template<typename S>
	S& getScene(SceneId id);

	template<typename T, typename ... Args>
	InputDeviceId loadInputDevice(Args ... args);

	template<typename P, typename ... Args>
	PacketId loadPacketHandler(PacketId id, Args... args);

	InputDevice& getInputDevice(InputDeviceId id);

	void setSceneFlags(SceneId id, Scene::FlagType flags_, bool value);
	void toggleSceneFlags(SceneId id, Scene::FlagType flags_);
	void sceneOn(SceneId id);
	void sceneOff(SceneId id);

	void unloadScene(SceneId id);
    void close();
	void tickTime();

	const double PHYSICS_STEP;
	const double RENDER_STEP;

	const RenderSystem& getRender();
	const EventQueue& getEvents();

	void setGameTick(Tick newGameTick);
    Tick getGameTick() const;
    
    void physicsUpdate();
	void onConnect(PeerId id);
	void addOwnedNetId(NetworkId id);
	void removeOwnedNetId(NetworkId id);
	const std::vector<NetworkId>& getOwnedNetIds() const;

	Host host;
	OnlineSystem online;
    SyncSystem sync;
	ServerInputQueue serverInputQueue;
	//how often, in game ticks, the client pings the server
	Tick clientPingDelay;
	//how often, in game ticks, the server broadcasts the state
	Tick serverBroadcastDelay;

	FlagType getFlags();
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
	Tick gameTick;
	Tick clientPingCtr;
	Tick serverBroadcastCtr;
	EventQueue events;

	RenderSystem renderSystem;
	std::vector<ScenePtr> scenes;
	std::unordered_map<InputDeviceId, InputDevicePtr> inputDevices;
	FlagType flags;

	//for a client, the network ids we own
	std::vector<NetworkId> ownedNetIds;
};

template<typename S, typename ... Args>
inline SceneId Game::loadScene(char flags_, Args... args) {
	ScenePtr scene = std::make_unique<S>(scenes.size(), flags_, args...);
	scene->load(*this);
	scenes.emplace_back(std::move(scene));
	return scenes.size() - 1;
}

template<typename S>
inline S& Game::getScene(SceneId id) {
	if (id >= 0 && scenes.size() > id) {
		return *static_cast<S*>(scenes[id].get());
	}
	else {
		//we're missing that id!
		throw std::exception{};
	}
}

template<typename T, typename ...Args>
inline InputDeviceId Game::loadInputDevice(Args ...args) {
	InputDevicePtr inputDevice = std::make_unique<T>(inputDevices.size());
	inputDevices.emplace(inputDevices.size(), std::move(inputDevice));
	return inputDevices.size() - 1;
}

template<typename P, typename ...Args>
inline PacketId Game::loadPacketHandler(PacketId id, Args ...args) {
	host.loadPacketHandler<P>(id, args...);
	return id;
}
