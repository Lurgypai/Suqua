#include "SuquaLib.h"
#include "../Box3Core/BoxScene.h"
#include "IDKeyboardMouse.h"
#include <iostream>

int main(int argc, char* argv[]) {
	SuquaLib::SuquaInit("settings.json", SuquaLib::network, { 640, 480 });
	Game game{ 1.0 / 120, 1.0 / 60, 1.0/30, Game::Flag::server_flags };
	SceneId boxScene = game.loadScene<BoxScene>(Scene::physics);
	SuquaLib::RunGame(game);

	return 0;
}