#include "SuquaLib.h"
#include "ServerBoxScene.h"
#include "IDKeyboardMouse.h"
#include "../Box3Core/BoxPacket.h"
#include "Game.h"
#include "EntityBaseComponent.h"
#include <iostream>

void disconnectCallback(Game& game, PeerId id) {
	ByteStream quitPacket{};
	quitPacket << BoxPacket::LeavePacket;
	//put the only net id this peer owns in
	NetworkId targetNetId = game.host.getPeerOwnedNetIds(id).front();
	quitPacket << targetNetId;

	game.host.bufferAllDataByChannel(0, quitPacket);
	std::cout << "Broadcasting removal of net id " << game.host.getPeerOwnedNetIds(id).front() << '\n';

	//erase the associated components
	EntitySystem::GetComp<EntityBaseComponent>(game.online.getEntity(targetNetId))->isDead = true;
	//remove from the networking
	game.host.removeNetIdFromPeer(id, targetNetId);
}

int main(int argc, char* argv[]) {
	SuquaLib::SuquaInit("Box3Server", "settings.json", SuquaLib::network);
	Game game{ Game::Flag::server_flags };
	game.serverBroadcastDelay = 12;
	game.host.setDisconnectCallback(disconnectCallback);
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

/*
removing entities should be added to scenes, to make the process cleaner
network callbacks should have the game, and the peer id, not the peer itself (end user shouldn't have to touch enet)
add onDisconnect to scenes? remove onConnect from scenes? whats the better option?
*/