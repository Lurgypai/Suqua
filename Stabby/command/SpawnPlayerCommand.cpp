#include "SpawnPlayerCommand.h"
#include "EntitySystem.h"
#include "PositionComponent.h"
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
	if (args.size() == 4) {
		int amount = std::stoi(args[1]);
		Vec2f pos{ std::stof(args[2]), std::stof(args[3]) };
		for (int i = 0; i != amount; ++i) {
			EntityId playerId = players->makePlayer(*weapons);

			EntitySystem::MakeComps<PlayerGC>(1, &playerId);
			EntitySystem::GetComp<RenderComponent>(playerId)->loadDrawable<AnimatedSprite>("character", Vec2i{ 64, 64 });
			EntitySystem::GetComp<PlayerGC>(playerId)->loadAnimations();
			EntitySystem::GetComp<PlayerGC>(playerId)->attackSprite = weapons->cloneAnimation("player_sword");

			EntitySystem::GetComp<CombatComponent>(playerId)->teamId = 2;

			EntitySystem::GetComp<PositionComponent>(playerId)->pos = pos;
		}
	}
}
