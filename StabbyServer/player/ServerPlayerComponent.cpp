
#include "ServerPlayerComponent.h"
#include "DebugFIO.h"
#include <algorithm>
#include <ControllerComponent.h>

ServerPlayerComponent::ServerPlayerComponent(EntityId id_) :
	id{id_},
	latestTime{0},
	clientTime{0},
	commands{},
	latestCommand{},
	timeIsSet{false}
{}

const EntityId ServerPlayerComponent::getId() const {
	return id;
}

Time_t ServerPlayerComponent::getClientTime() {
	return clientTime;
}

void ServerPlayerComponent::bufferInput(ClientCommand c) {
	//if its the first input, set the start for the client time
	//only discard inputs from 1 server tick ago

	//skip inputs older than the last one
	//keep inputs older than the current time (clientTime). Avoid dropping inputs (though this will cause desyncs if thigns are too old.)

	commands.push_back(c);
	if(c.clientTime > latestTime)
		latestTime = c.clientTime;
}

//its sending both before and after toggle (though you changed that) the problem is, sometimes before toggle is ignored in favor of after toggle.
ClientCommand ServerPlayerComponent::readCommand(Time_t gameTime) {

	DebugFIO::Out("s_out.txt") << "Looking for command for time " << clientTime << '\n';
	for (auto command = commands.begin(); command != commands.end();) {
		if (command->clientTime <= clientTime) {
			if (command->clientTime > latestCommand.clientTime) {
				DebugFIO::Out("s_out.txt") << "Updating latest command to " << static_cast<int>(command->controllerState.getState()) << ", " << static_cast<int>(command->controllerState.getPrevState()) << " from time " << command->clientTime << '\n';
				latestCommand = *command;
			}
			command = commands.erase(command);
		}
		else {
			DebugFIO::Out("s_out.txt") << "Ignoring command " << static_cast<int>(command->controllerState.getState()) << ", " << static_cast<int>(command->controllerState.getPrevState()) << " labelled as " << command->clientTime << '\n';
			++command;
		}
	}
	DebugFIO::Out("s_out.txt") << "Latest command is " << static_cast<int>(latestCommand.controllerState.getState()) << ", " << static_cast<int>(latestCommand.controllerState.getPrevState()) << " labelled as " << latestCommand.clientTime << " for time " << clientTime << '\n';
	DebugFIO::Out("s_out.txt") << '\n';

	PlayerStateComponent* stateComp = EntitySystem::GetComp<PlayerStateComponent>(id);
	stateComp->playerState.gameTime = gameTime;
	stateComp->playerState.clientTime = clientTime;

	ControllerComponent* controller = EntitySystem::GetComp<ControllerComponent>(id);
	controller->getController().setState(latestCommand.controllerState.getState());
	DebugFIO::Out("s_out.txt") << "Final controller state used is " << static_cast<int>(controller->getController().getState()) << ", " << static_cast<int>(controller->getController().getPrevState()) << '\n';

	//DebugFIO::Out("s_out.txt") << "Used command: " << static_cast<int>(latestCommand.controllerState.getState()) << " for time " << clientTime << '\n';
	//return the command with a time equal to or less than (the most recent thats not after) the current time
	return latestCommand;
}

void ServerPlayerComponent::tickClientTime() {
	++clientTime;
}

void ServerPlayerComponent::storeGameState(const GameState& state) {
	prevStates.push_front(state);
	if (prevStates.size() > 30)
		prevStates.pop_back();
}

void ServerPlayerComponent::acknowledgeState(GameStateId id) {
	for (auto& state : prevStates) {
		if (state.getId() == id) {
			state.acknowledge();
		}
	}
}

bool ServerPlayerComponent::getLastAcknowledged(GameState& retState) {
	for (const auto& state : prevStates) {
		if (state.getAcknowledged()) {
			retState = state;
			return true;
		}
	}
	return false;
}

bool ServerPlayerComponent::getTimeIsSet() const {
	return timeIsSet;
}

void ServerPlayerComponent::setTime(Time_t clientTime_) {
	timeIsSet = true;
	clientTime = clientTime_;
}
