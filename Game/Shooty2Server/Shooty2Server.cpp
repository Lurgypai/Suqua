#include <fstream>

#include "nlohmann/json.hpp"

#include "SuquaLib.h"
#include "ServerWorldScene.h"
#include "DebugFIO.h"
#include "../Shooty2Core/ItemSystem.h"

using json = nlohmann::json;

int main(int argc, char** argv) {
	SuquaLib::SuquaInit("Shooty2", "settings.json", SuquaLib::network);

	Game game{ Game::server_flags };

    DebugFIO::AddFOut("debug.log");

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
    game.setStateBroadcastDelay(4);

    ItemSystem items;
    items.loadItems("item/items.json");
	SceneId lobbyScene = game.loadScene<ServerWorldScene>(Scene::Flag::physics | Scene::Flag::input, items, "levels/test.ldtk");

	SuquaLib::RunGame(game);

	return 0;
}
