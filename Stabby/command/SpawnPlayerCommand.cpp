#include "SpawnPlayerCommand.h"
#include "EntitySystem.h"
#include "PositionComponent.h"
#include "../graphics/PlayerGC.h"
#include "player.h"

SpawnPlayerCommand::SpawnPlayerCommand(Game* game_) :
	game{game_}
{}

std::string SpawnPlayerCommand::getTag() const {
	return "spawnplayer";
}

void SpawnPlayerCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 4) {
		int amount = std::stoi(args[1]);
		Vec2f pos{ std::stof(args[2]), std::stof(args[3]) };
		for (int i = 0; i != amount; ++i) {
			EntityId playerId = game->players.makePlayer(game->weapons);

			game->makePlayerGFX(playerId);

			EntitySystem::GetComp<CombatComponent>(playerId)->teamId = 2;

			EntitySystem::GetComp<PositionComponent>(playerId)->pos = pos;

			EntitySystem::MakeComps<AIPlayerComponent>(1, &playerId);
		}
	}
}
