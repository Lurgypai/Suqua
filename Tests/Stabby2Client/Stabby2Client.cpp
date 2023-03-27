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
	SceneId playingScene = game.loadScene<ClientPlayingScene>(Scene::Flag::all, keyboardId);
	SceneId menuScene = game.loadScene<MenuScene>(Scene::Flag::all, playingScene, ip);

	game.loadPacketHandler<JoinPacketHandler>(Stabby2Packet::JoinPacket, playingScene);
	game.loadPacketHandler<OwnedNetIdPacketHandler>(Stabby2Packet::OwnedNetIdPacket, playingScene);

	constexpr bool online = true;
	if (online) {
		game.sceneOff(playingScene);
	}
	else {
		auto playerId = game.getScene<ClientPlayingScene>(playingScene).addPlayer();
		game.getScene<ClientPlayingScene>(playingScene).addControl(playerId);
		auto zombieId = game.getScene<ClientPlayingScene>(playingScene).addZombie(game, { 200, -10 });
		game.sceneOff(menuScene);

	}

	SuquaLib::RunGame(game);

	return 0;
}