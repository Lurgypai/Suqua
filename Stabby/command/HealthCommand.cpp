#include "HealthCommand.h"
#include "combat.h"
#include "NetworkDataComponent.h"

std::string HealthCommand::getTag() const {
	return "health";
}

void HealthCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 2) {
		int amount = std::stoi(args[1]);
		for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
			NetworkDataComponent* data = EntitySystem::GetComp<NetworkDataComponent>(player.getId());
			data->get<int32_t>(HEALTH) = amount;
		}
	}
}
