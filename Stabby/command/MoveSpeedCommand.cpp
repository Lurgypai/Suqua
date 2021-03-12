
#include "MoveSpeedCommand.h"
#include "DebugIO.h"
#include "NetworkDataComponent.h"
#include "player.h"

MoveSpeedCommand::MoveSpeedCommand(Game & game_) :
	game{game_}
{}

std::string MoveSpeedCommand::getTag() const {
	return "movespeed";
}

void MoveSpeedCommand::onCommand(const std::vector<std::string>& args) {

	if (args.size() != 2) {
		DebugIO::printLine("Incorrect amount of args.");
	}
	else {
		try {
			double speed = std::stod(args[1]);
			NetworkDataComponent* data = EntitySystem::GetComp<NetworkDataComponent>(game.getPlayerId());
			data->get<double>(MOVE_SPEED) = speed;
		}
		catch (std::invalid_argument e) {
			DebugIO::printLine("Invalid argument. Needs a double.");
		}
	}
}
