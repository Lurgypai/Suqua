#include "SDL.h"
#include "SuquaLib.h"
#include "IDKeyboardMouse.h"
#include "ClientPlayingScene.h"
#include <fstream>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

int main(int argc, char** argv) {
	SuquaLib::SuquaInit("Stabby2", "settings.json", SuquaLib::all);
	
	Game game{ Game::client_flags };
	game.serverBroadcastDelay = 0;

	//json settings;
	//std::ifstream file{ "settings.json" };
	//Vec2i windowRes;
	//if (file.good()) {
	//	file >> settings;
	//	file.close();
	//}

	//std::string ip = "127.0.0.1";
	//if (settings.contains("ip")) {
	//	ip = settings["ip"];
	//}

	//std::cout << "The target ip is " << ip << ".\n";

	InputDeviceId keyboardId = game.loadInputDevice<IDKeyboardMouse>();

	SceneId playingScene = game.loadScene<ClientPlayingScene>(Scene::Flag::all, keyboardId);
	game.getScene<ClientPlayingScene>(playingScene).addPlayer();

	SuquaLib::RunGame(game);

	return 0;
}