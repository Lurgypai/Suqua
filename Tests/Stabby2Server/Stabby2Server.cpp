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

	json settings;
	std::ifstream file{ "settings.json" };
	if (file.good()) {
		file >> settings;
		file.close();
	}

	if (settings.contains("networkInputDelay")) {
		game.networkInputDelay = settings["networkInputDelay"];
		// given an input delay of n, we won't receive inputs for frames 0 to n-1
		game.setGameTick(game.networkInputDelay);
	}
	std::cout << "The network input delay is " << game.networkInputDelay << ".\n";

	SceneId playingSceneId = game.loadScene<PlayingScene>(Scene::Flag::physics);
	game.loadScene<LobbyScene>(Scene::Flag::physics, playingSceneId, 1);
	game.sceneOff(playingSceneId);
	
	SuquaLib::RunGame(game);

	return 0;
}