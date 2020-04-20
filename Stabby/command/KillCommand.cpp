#include "KillCommand.h"
#include "player.h"

std::string KillCommand::getTag() const {
	return "kill";
}

void KillCommand::onCommand(const std::vector<std::string>& args) {
	for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
		player.kill();
	}
}
