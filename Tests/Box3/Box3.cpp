#include "SuquaLib.h"
#include "ClientBoxScene.h"
#include "PauseScene.h"
#include "IDKeyboardMouse.h"
#include "PHBoxClientNetworkId.h"
#include "PHBoxJoin.h"
#include "PHBoxQuit.h"
#include "../Box3Core/BoxPacket.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>

using nlohmann::json;

void connectionCallback(Game& game, PeerId id) {
	std::cout << "Called connection callback on client\n";
}

int main(int argc, char* argv[]) {
	//view res needs to be removed
	SuquaLib::SuquaInit("Box3", "settings.json", SuquaLib::all);
	Game game{ Game::Flag::client_flags };
	game.serverBroadcastDelay = 12;
	InputDeviceId keyboard = game.loadInputDevice<IDKeyboardMouse>();

	json settings;
	std::ifstream file{ "settings.json" };
	Vec2i windowRes;
	if (file.good()) {
		file >> settings;
		file.close();
	}

	std::string ip = "127.0.0.1";
	if (settings.contains("ip")) {
		ip = settings["ip"];
	}

	std::cout << "The target ip is " << ip << ".\n";

	SceneId boxScene = game.loadScene<ClientBoxScene>(Scene::all, keyboard, ip, 25565);
	game.loadScene<PauseScene>(Scene::physics, boxScene);
	game.loadPacketHandler<PHBoxClientNetworkId>(BoxPacket::ClientNetworkIdPacket, boxScene);
	game.loadPacketHandler<PHBoxJoin>(BoxPacket::JoinPacket, boxScene);
	game.loadPacketHandler<PHBoxQuit>(BoxPacket::LeavePacket);

	game.host.setConnectCallback(&connectionCallback);
	SuquaLib::RunGame(game);

	return 0;
}
