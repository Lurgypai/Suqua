
#include "WeaponCommand.h"
#include "DebugIO.h"

#include "../graphics/PlayerGC.h"

WeaponCommand::WeaponCommand(Game & game) :
	weapons(&game.weapons),
	client{&game.client}
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
					CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(id);
					PlayerGC* graphics = EntitySystem::GetComp<PlayerGC>(id);

					combat->attack = weapons->cloneAttack(args[1]);
					graphics->attackSprite = weapons->cloneAnimation(args[1]);

					if (client->getConnected()) {
						OnlineComponent* online = EntitySystem::GetComp<OnlineComponent>(id);
						size_t size = args[1].size();

						WeaponChangePacket p;
						p.size = size;
						p.id = online->getNetId();
						p.serialize();
						char* data = static_cast<char*>(malloc(sizeof(WeaponChangePacket) + size));
						memcpy(data, &p, sizeof(WeaponChangePacket));
						memcpy(data + sizeof(WeaponChangePacket), args[1].data(), size);
						client->send(sizeof(WeaponChangePacket) + size, data);
						free(data);
					}
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
