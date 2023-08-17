#include <fstream>

#include "SDL.h"
#include "nlohmann/json.hpp"

#include "SuquaLib.h"
#include "IDKeyboardMouse.h"
#include "ClientPlayingScene.h"
#include "MenuScene.h"
#include "JoinPacketHandler.h"
#include "OwnedNetIdPacketHandler.h"

#include "../Stabby2Core/Stabby2Packet.h"

using json = nlohmann::json;

int main(int argc, char** argv) {
	SuquaLib::SuquaInit("Stabby2", "settings.json", SuquaLib::all);
	GLRenderer::LoadTexture("none.png", "none");
	
	Game game{ Game::client_flags };
	game.serverBroadcastDelay = 0;

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

	std::cout << "The target ip is " << ip << ".\n";

	InputDeviceId keyboardId = game.loadInputDevice<IDKeyboardMouse>();
	SceneId playingScene = game.loadScene<ClientPlayingScene>(Scene::Flag::all, keyboardId);
	SceneId menuScene = game.loadScene<MenuScene>(Scene::Flag::all, playingScene, ip);

	game.loadPacketHandler<JoinPacketHandler>(Stabby2Packet::JoinPacket, playingScene);
	game.loadPacketHandler<OwnedNetIdPacketHandler>(Stabby2Packet::OwnedNetIdPacket, playingScene);

	constexpr bool online = false;
	if (online) {
		game.sceneOff(playingScene);
	}
	else {
		auto playerId = game.getScene<ClientPlayingScene>(playingScene).addPlayer();
		game.getScene<ClientPlayingScene>(playingScene).addControl(playerId);
		game.sceneOff(menuScene);

	}

	SuquaLib::RunGame(game);

	return 0;
}