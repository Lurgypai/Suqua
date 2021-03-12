#include "SpawnPlayerCommand.h"
#include "EntitySystem.h"
#include "NetworkDataComponent.h"
#include "../graphics/PlayerGC.h"
#include "player.h"
#include "PositionData.h"
#include "combat.h"

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

			NetworkDataComponent* data = EntitySystem::GetComp<NetworkDataComponent>(playerId);
			data->get<float>(X) = pos.x;
			data->get<float>(Y) = pos.y;

			data->get<uint32_t>(TEAM_ID) = 2;

			EntitySystem::MakeComps<AIPlayerComponent>(1, &playerId);
		}
	}
}
