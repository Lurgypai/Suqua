#include "SuquaLib.h"
#include "ServerBoxScene.h"
#include "IDKeyboardMouse.h"
#include <iostream>

void connectCallback(const ENetEvent& e) {
	std::cout << "Called connect callback on server.\n";
}

int main(int argc, char* argv[]) {
	SuquaLib::SuquaInit("settings.json", SuquaLib::network, { 640, 480 });
	Game game{ 1.0 / 120, 1.0 / 60, 1.0/30, Game::Flag::server_flags };
	SceneId boxScene = game.loadScene<ServerBoxScene>(Scene::physics);
	SuquaLib::RunGame(game);

	return 0;
}

/*

change the boxscene and such to not generate the box.
add callback to server to notify clients that a player has joined, and generate the new boxes.
prepare server input roll-backers

*/

/*
the server needs to create boxes when a new client joins
this means that the callback needs to know about the box scene, or the box scene needs to know about the callback
*/