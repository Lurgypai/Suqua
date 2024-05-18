#include <fstream>

#include "SDL.h"
#include "nlohmann/json.hpp"

#include "SuquaLib.h"
#include "IDKeyboardMouse.h"
#include "WorldScene.h"

using json = nlohmann::json;

int main(int argc, char** argv) {
	SuquaLib::SuquaInit("Ishparsh", "settings.json", SuquaLib::all);
	GLRenderer::LoadTexture("suqua/images/none.png", "none");
	
	Game game{ Game::client_flags};
	game.serverBroadcastDelay = 0;
	game.clientPingDelay = 120;

	json settings;
	std::ifstream file{ "settings.json" };
	if (file.good()) {
		file >> settings;
		file.close();
	}

	SceneId playingScene = game.loadScene<WorldScene>(Scene::Flag::all);

	SuquaLib::RunGame(game);

	return 0;
}
