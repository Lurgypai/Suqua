#include "SpawnPlayerCommand.h"
#include "EntitySystem.h"
#include "../graphics/PlayerGC.h"
#include "player.h"

SpawnPlayerCommand::SpawnPlayerCommand(PlayerManager* players_, WeaponManager* weapons_) :
	players{ players_ },
	weapons{ weapons_ }
{}

std::string SpawnPlayerCommand::getTag() const {
	return "spawnplayer";
}

void SpawnPlayerCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 2) {
		int amount = std::stoi(args[1]);
		for (int i = 0; i != amount; ++i) {
			EntityId playerId = players->makePlayer(*weapons);

			EntitySystem::MakeComps<PlayerGC>(1, &playerId);
			EntitySystem::GetComp<RenderComponent>(playerId)->loadDrawable<AnimatedSprite>("images/stabbyman.png", Vec2i{ 64, 64 });
			EntitySystem::GetComp<PlayerGC>(playerId)->loadAnimations();
			EntitySystem::GetComp<PlayerGC>(playerId)->attackSprite = weapons->cloneAnimation("player_sword");

			EntitySystem::GetComp<PlayerLC>(playerId)->chooseSpawn();
			EntitySystem::GetComp<CombatComponent>(playerId)->teamId = 2;
		}
	}
}
