
#include "nlohmann/json.hpp"

#include "SuquaLib.h"
#include "WorldScene.h"

using json = nlohmann::json;

int main(int argc, char** argv) {
    // Initialize the library. This generates a window, using the resolution specified in the settings. The flags here specify if rendering support should be enabled.
    // See SuquaLib.h for the available flags

	SuquaLib::SuquaInit("Basic Rendering Test", "settings.json", SuquaLib::graphics);
    //None is a texture used when the requested texture cannot be found. This code should probably be moved somewhere else, for now ignore it.
	GLRenderer::LoadTexture("suqua/images/none.png", "none");
	
    // Create an instance of the game master class, set up as a client
	Game game{ Game::client_flags};

    // load the scene to start the game on.
	game.loadScene<WorldScene>(Scene::Flag::all);

    // begin the game loop
	SuquaLib::RunGame(game);

	return 0;
}
