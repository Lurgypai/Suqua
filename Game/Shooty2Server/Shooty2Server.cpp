#include <fstream>

#include "nlohmann/json.hpp"

#include "SuquaLib.h"
#include "ServerWorldScene.h"
#include "DebugFIO.h"

using json = nlohmann::json;

int main(int argc, char** argv) {
	SuquaLib::SuquaInit("Shooty2", "settings.json", SuquaLib::network);

	Game game{ Game::server_flags, 1.f / 30.f};
	game.serverBroadcastDelay = 0;
	game.clientPingDelay = 120;

    DebugFIO::AddFOut("send.packet.log");

	json settings;
	std::ifstream file{ "settings.json" };
	if (file.good()) {
		file >> settings;
		file.close();
	}

    if(settings.contains("debugNetDelay")) {
        int delayMin = settings["debugNetDelay"]["min"];
        int delayVar = settings["debugNetDelay"]["variation"];
        game.host.enableDelay(delayMin, delayVar);

        std::cout << "Enabled debug delay, minimum " << delayMin << ", variation " << delayVar << '\n';
    }

	std::cout << "The network input delay is " << game.networkInputDelay << ".\n";

	SceneId lobbyScene = game.loadScene<ServerWorldScene>(Scene::Flag::physics | Scene::Flag::input);

	SuquaLib::RunGame(game);

	return 0;
}
