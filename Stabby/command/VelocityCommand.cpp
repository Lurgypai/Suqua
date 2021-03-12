#include "VelocityCommand.h"
#include "DebugIO.h"
#include "EntitySystem.h"
#include "player.h"
#include "NetworkDataComponent.h"

std::string VelocityCommand::getTag() const {
	return "velocity";
}

void VelocityCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 3) {
		float x = std::stof(args[1]);
		float y = std::stof(args[2]);

		for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
			NetworkDataComponent* data = EntitySystem::GetComp<NetworkDataComponent>(player.getId());
			data->get<float>(XVEL) = x;
			data->get<float>(YVEL) = y;
		}
	}
	else {
		DebugIO::printLine("Incorrect amount of args!");
	}
}
