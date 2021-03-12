
#include "WeaponCommand.h"
#include "DebugIO.h"

#include "../graphics/PlayerGC.h"

WeaponCommand::WeaponCommand(Game & game) :
	weapons(&game.weapons)
{}

std::string WeaponCommand::getTag() const {
	return "weapon";
}

void WeaponCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 2) {
		if (weapons->hasWeapon(args[1])) {
			if (EntitySystem::Contains<PlayerLC>() && EntitySystem::Contains<PlayerGC>()) {
				for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
					EntityId id = player.getId();
					PlayerLC* player = EntitySystem::GetComp<PlayerLC>(id);
					PlayerGC* graphics = EntitySystem::GetComp<PlayerGC>(id);

					player->setWeapon(args[1]);
				}
			}
		}
		else {
			DebugIO::printLine("Unable to find weapon.");
		}
	}
	else {
		DebugIO::printLine("Incorrect amount of arguments.");
	}
}
