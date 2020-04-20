
#include "TeleportCommand.h"
#include "player.h"
#include <DebugIO.h>

std::string TeleportCommand::getTag() const {
	return "tp";
}

void TeleportCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 3) {
		if (EntitySystem::Contains<PlayerLC>()) {
			for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
				PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(player.getId());
				Vec2f pos{std::stof(args[1]), std::stof(args[2])};
				physics->teleport(pos);
				DebugIO::printLine("Teleported to " + std::to_string(pos.x) + ", " + std::to_string(pos.y));
			}
		}
	}
}
