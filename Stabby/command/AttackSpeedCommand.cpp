#include "DebugIO.h"
#include "AttackSpeedCommand.h"

AttackSpeedCommand::AttackSpeedCommand(Game & game_) : game{game_} {}

std::string AttackSpeedCommand::getTag() const {
	return "attackspeed";
}

void AttackSpeedCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() != 2) {
		DebugIO::printLine("Incorrect amount of args.");
	}
	else {
		try {
			double speed = std::stod(args[1]);
			CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(game.getPlayerId());
			combat->setAttackSpeed(speed);
		}
		catch (std::invalid_argument e) {
			DebugIO::printLine("Invalid argument. Needs a double.");
		}
	}
}
