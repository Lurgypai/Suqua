#include <fstream>

#include "nlohmann/json.hpp"

#include "SuquaLib.h"
#include "ClientWorldScene.h"
#include "MenuScene.h"
#include "DebugFIO.h"
#include "IDKeyboardMouse.h"

#include "InterfaceItemGFXSystem.h"

using json = nlohmann::json;

int main(int argc, char** argv) {
    /* Library Init */
	SuquaLib::SuquaInit("Shooty2", "settings.json", SuquaLib::all);

	
    /* Game Init */
	Game game{ Game::client_flags };
    game.setStateBroadcastDelay(4);

    DebugFIO::AddFOut("debug.log");

    // load settings
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
    if(settings.contains("debugNetDelay")) {
        int delayMin = settings["debugNetDelay"]["min"];
        int delayVar = settings["debugNetDelay"]["variation"];
        game.host.enableDelay(delayMin, delayVar);

        std::cout << "Enabled debug delay, minimum " << delayMin << ", variation " << delayVar << '\n';
    }
    game.setStateBroadcastDelay(4);
	std::cout << "The target ip is " << ip << ".\n";

    /* Load Objects */
    // Items
    ItemSystem items;
    items.loadItems("item/items.json");

    InterfaceItemGFXSystem interfaceItemGfx;
    interfaceItemGfx.loadGfx(items);

    HandItemGFXSystem handItemGfx;
    handItemGfx.loadGfx(items);

    /* Setup Scenes */
    game.host.tryConnect(ip, 25565, 10);
    InputDeviceId input = game.loadInputDevice<IDKeyboardMouse>();
	SceneId playingSceneId = game.loadScene<ClientWorldScene>(Scene::Flag::all, input, items, handItemGfx, "levels/debug.ldtk");
    auto& playingScene = game.getScene<ClientWorldScene>(playingSceneId);
    SceneId menuScene = game.loadScene<MenuScene>(Scene::Flag::none, playingSceneId, input, items, interfaceItemGfx,
            playingScene.myPlayerId, playingScene.myDaemonId);
    playingScene.menuScene = menuScene;

	SuquaLib::RunGame(game);

	return 0;
}

// review how the stage system works, and set it up server side
// director
//  add network mode
//  set network mode to send teleport packets to clients
//  set server scene to add/remove players to director as they're generated
//      have clients request their players are added to the director? thats probably the cleanest way
//      maybe we create a "session" that just maintains a list of the current primary player controlled entities
//      anything that needs to track or find players can use the session?
//      when we're offline we just start a local session as opposed to an online one? hmm...
// client is ready packet to prevent weird spawn without getting moved things
//
// fix teleporting
// fix dealing damage (damage packet is needed). setting can cause two client to overwrite
// fix packet desync
//  log serialize and unserialize, and damage packet
// new bug, moving is getting the wrong size.
//  log the order packets are sent
//  log the sent data
