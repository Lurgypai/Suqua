#include "SuquaLib.h"
#include "BoxScene.h"
#include "PauseScene.h"
#include "IDKeyboardMouse.h"
#include <iostream>

int main(int argc, char* argv[]) {
	SuquaLib::SuquaInit("settings.json", SuquaLib::all, { 640, 480 });
	Game game{1.0 / 120, 1.0 / 60, 0, Game::Flag::client_flags};
	InputDeviceId keyboard = game.loadInputDevice<IDKeyboardMouse>();
	SceneId boxScene = game.loadScene<BoxScene>(Scene::all, keyboard);
	game.loadScene<PauseScene>(Scene::physics, boxScene);
	SuquaLib::RunGame(game);

	std::cout << "Hello, World!";

	return 0;
}