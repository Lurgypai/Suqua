#include <iostream>

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

	SceneId playingSceneId = game.loadScene<PlayingScene>(Scene::Flag::physics);
	game.loadScene<LobbyScene>(Scene::Flag::physics, playingSceneId);
	game.sceneOff(playingSceneId);
	
	SuquaLib::RunGame(game);

	return 0;
}