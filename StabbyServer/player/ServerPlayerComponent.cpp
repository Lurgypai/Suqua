
#include "ServerPlayerComponent.h"
#include "DebugFIO.h"
#include <algorithm>
#include <ControllerComponent.h>

ServerPlayerComponent::ServerPlayerComponent(EntityId id_) :
	id{id_},
	latestTime{0},
	clientTime{0},
	commands{},
	latestCommand{}
{}

const EntityId ServerPlayerComponent::getId() const {
	return id;
}

Time_t ServerPlayerComponent::getClientTime() {
	return clientTime;
}

void ServerPlayerComponent::bufferInput(ClientCommand c) {
	//skip inputs older than the last one
	//keep inputs older than the current time (clientTime). Avoid dropping inputs (though this will cause desyncs if thigns are too old.)
	if (c.clientTime > latestTime) {

		Time_t delay = ((1 / CLIENT_TIME_STEP) * SERVER_TIME_STEP);
		if (c.clientTime < clientTime || clientTime < c.clientTime - (delay * 2)) {
			//delay by one server tick, so all incoming updates are properly in time.
			clientTime = c.clientTime - delay;
		}

		DebugFIO::Out("s_out.txt") << "Received controller state for time: " << c.clientTime << ", state: " << static_cast<int>(c.controllerState.getState()) << '\n';

		commands.push_back(c);
		latestTime = c.clientTime;
	}
}

ClientCommand ServerPlayerComponent::readCommand(Time_t gameTime) {

	for (auto command = commands.begin(); command != commands.end();) {
		if (command->clientTime <= clientTime) {
			if (command->clientTime > latestCommand.clientTime)
				latestCommand = *command;
			command = commands.erase(command);
		}
		else {
			++command;
		}
	}

	PlayerStateComponent* stateComp = EntitySystem::GetComp<PlayerStateComponent>(id);
	stateComp->playerState.gameTime = gameTime;
	stateComp->playerState.clientTime = clientTime;

	ControllerComponent* controller = EntitySystem::GetComp<ControllerComponent>(id);
	controller->getController() = Controller{ latestCommand.controllerState };

	DebugFIO::Out("s_out.txt") << "Used command: " << static_cast<int>(latestCommand.controllerState.getState()) << " for time " << clientTime << '\n';
	//return the command with a time equal to or less than (the most recent thats not after) the current time
	return latestCommand;
}

void ServerPlayerComponent::tickClientTime() {
	++clientTime;
}
