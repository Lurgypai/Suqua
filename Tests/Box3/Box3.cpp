#include "SuquaLib.h"
#include "ClientBoxScene.h"
#include "PauseScene.h"
#include "ConnectScene.h"
#include "IDKeyboardMouse.h"
#include <iostream>

int main(int argc, char* argv[]) {
	SuquaLib::SuquaInit("settings.json", SuquaLib::all, { 640, 480 });
	Game game{1.0 / 120, 1.0 / 60, 0, Game::Flag::client_flags};
	InputDeviceId keyboard = game.loadInputDevice<IDKeyboardMouse>();
	SceneId boxScene = game.loadScene<ClientBoxScene>(Scene::all, keyboard);
	game.loadScene<PauseScene>(Scene::physics, boxScene);
	game.loadScene<ConnectScene>(Scene::physics, "127.0.0.1", 25565);
	SuquaLib::RunGame(game);

	return 0;
}
