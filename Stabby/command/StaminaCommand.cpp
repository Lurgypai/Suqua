#include "StaminaCommand.h"

StaminaCommand::StaminaCommand(Game& game_) : game{game_}
{}

std::string StaminaCommand::getTag() const
{
	return "stamina";
}

void StaminaCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 2) {
		int stamina = std::stoi(args[1]);
		CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(game.getPlayerId());
		combat->stamina = stamina;
	}
}
