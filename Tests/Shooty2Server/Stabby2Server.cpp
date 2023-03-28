#include <iostream>
#include <fstream>

#include "SDL.h"
#include "nlohmann/json.hpp"

#include "SuquaLib.h"
#include "LobbyScene.h"
#include "../Stabby2Core/PlayingScene.h"

using json = nlohmann::json;

int main(int argc, char** argv) {
	SuquaLib::SuquaInit("Stabby2", "settings.json", SuquaLib::network);

	Game game{ Game::server_flags };
	game.serverBroadcastDelay = 0;

	// set the timeout to one physics tick, so it should stay roughly in sync even when delayed
	game.networkInputTimeout = 2.0 / 120;

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

	if (settings.contains("networkInputTimeout")) {
		game.networkInputTimeout = settings["networkInputTimeout"];
		game.setGameTick(game.networkInputTimeout);
	}
	std::cout << "The network input timeout is " << game.networkInputDelay << ".\n";

	SceneId playingSceneId = game.loadScene<PlayingScene>(Scene::Flag::physics);
	game.loadScene<LobbyScene>(Scene::Flag::physics, playingSceneId, 1);
	game.sceneOff(playingSceneId);
	
	SuquaLib::RunGame(game);

	return 0;
}