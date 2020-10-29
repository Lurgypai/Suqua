#pragma once
#include <deque>

#include "gamestate.h"

#include "EntitySystem.h"
#include "Controller.h"

#include "network.h"

struct ClientCommand {
	Controller controllerState;
	Time_t clientTime;
	Time_t gameTime;
};

class ServerPlayerComponent {
public:
	ServerPlayerComponent(EntityId id_ = 0);
	const EntityId getId() const;

	Time_t getClientTime();
	void bufferInput(ClientCommand c);
	ClientCommand readCommand(Time_t gameTime);
	void tickClientTime();
	void storeGameState(const GameState& state);
	void acknowledgeState(GameStateId id);
	bool getLastAcknowledged(GameState& state);
private:
	bool firstInput;
	Time_t latestTime;
	Time_t clientTime;
	std::deque<ClientCommand> commands;
	std::deque<GameState> prevStates;
	ClientCommand latestCommand;

	EntityId id;
};