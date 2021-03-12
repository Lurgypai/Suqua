#include "SuquaLib.h"
#include "BoxScene.h"

int main(int argc, char* argv[]) {
	SuquaLib::SuquaInit("settings.json", 0, { 640, 480 });
	Game game{1.0 / 120, 1.0 / 60};
	game.loadScene<BoxScene>();
	SuquaLib::RunGame(game);

	return 0;
}