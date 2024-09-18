#include <fstream>

#include "SDL.h"
#include "nlohmann/json.hpp"

#include "SuquaLib.h"
#include "LobbyScene.h"

using json = nlohmann::json;

int main(int argc, char** argv) {
	SuquaLib::SuquaInit("Shooty2", "settings.json", SuquaLib::network);

	Game game{ Game::server_flags };
	game.serverBroadcastDelay = 0;
	game.clientPingDelay = 120;

	json settings;
	std::ifstream file{ "settings.json" };
	if (file.good()) {
		file >> settings;
		file.close();
	}

	if (settings.contains("networkInputDelay")) {
		game.networkInputDelay = settings["networkInputDelay"];
	}

	std::cout << "The network input delay is " << game.networkInputDelay << ".\n";

    SceneId playingId = 0;
	SceneId lobbyScene = game.loadScene<LobbyScene>(Scene::Flag::all, playingId, 2);

	SuquaLib::RunGame(game);

	return 0;
}
