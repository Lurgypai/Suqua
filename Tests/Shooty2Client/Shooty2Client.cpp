#include <fstream>

#include "SDL.h"
#include "nlohmann/json.hpp"

#include "SuquaLib.h"
#include "IDKeyboardMouse.h"
#include "ClientWorldScene.h"

using json = nlohmann::json;

int main(int argc, char** argv) {
	SuquaLib::SuquaInit("Shooty2", "settings.json", SuquaLib::all);
	GLRenderer::LoadTexture("suqua/images/none.png", "none");
	
	Game game{ Game::client_flags };
	game.serverBroadcastDelay = 0;
	game.clientPingDelay = 120;

	json settings;
	std::ifstream file{ "settings.json" };
	if (file.good()) {
		file >> settings;
		file.close();
	}

	std::string ip = "127.0.0.1";
	if (settings.contains("ip")) {
		ip = settings["ip"];
	}

	if (settings.contains("networkInputDelay")) {
		game.networkInputDelay = settings["networkInputDelay"];
	}

	std::cout << "The target ip is " << ip << ".\n";
	std::cout << "The network input delay is " << game.networkInputDelay << ".\n";

	InputDeviceId keyboardId = game.loadInputDevice<IDKeyboardMouse>();
	SceneId playingScene = game.loadScene<ClientWorldScene>(Scene::Flag::all, keyboardId);

	SuquaLib::RunGame(game);

	return 0;
}