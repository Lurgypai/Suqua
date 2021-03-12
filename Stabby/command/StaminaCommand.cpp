#include "StaminaCommand.h"
#include "NetworkDataComponent.h"
#include "combat.h"

StaminaCommand::StaminaCommand(Game& game_) : game{game_}
{}

std::string StaminaCommand::getTag() const
{
	return "stamina";
}

void StaminaCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 2) {
		int stamina = std::stoi(args[1]);
		NetworkDataComponent* data = EntitySystem::GetComp<NetworkDataComponent>(game.getPlayerId());
		data->get<uint32_t>(STAMINA) = stamina;
	}
}
