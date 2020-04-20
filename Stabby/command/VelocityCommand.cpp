#include "VelocityCommand.h"
#include "DebugIO.h"
#include "EntitySystem.h"
#include "player.h"

std::string VelocityCommand::getTag() const {
	return "velocity";
}

void VelocityCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 3) {
		float x = std::stof(args[1]);
		float y = std::stof(args[2]);

		for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
			PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(player.getId());
			physics->accelerate({ x, y });
		}
	}
	else {
		DebugIO::printLine("Incorrect amount of args!");
	}
}
