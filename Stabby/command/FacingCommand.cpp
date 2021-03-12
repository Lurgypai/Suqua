#include "FacingCommand.h"
#include "DebugIO.h"
#include "EntitySystem.h"
#include "player.h"
#include "NetworkDataComponent.h"
#include "DirectionData.h"

std::string FacingCommand::getTag() const {
	return "facing";
}

void FacingCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 2) {
		int x = std::stoi(args[1]);

		for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
			NetworkDataComponent* data = EntitySystem::GetComp<NetworkDataComponent>(player.getId());
			data->get<int32_t>(DIR) = x;
		}
	}
	else {
		DebugIO::printLine("Incorrect amount of args!");
	}
}
