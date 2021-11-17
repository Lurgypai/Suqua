#include "SuquaLib.h"
#include "ClientBoxScene.h"
#include "PauseScene.h"
#include "IDKeyboardMouse.h"
#include "PHBoxClientNetworkId.h"
#include "PHBoxJoin.h"
#include "../Box3Core/BoxPacket.h"
#include <iostream>

void connectionCallback(const ENetEvent& e) {
	std::cout << "Called connection callback on client\n";
}

int main(int argc, char* argv[]) {
	SuquaLib::SuquaInit("settings.json", SuquaLib::all, { 640, 480 });
	Game game{ Game::Flag::client_flags };
	InputDeviceId keyboard = game.loadInputDevice<IDKeyboardMouse>();
	SceneId boxScene = game.loadScene<ClientBoxScene>(Scene::all, keyboard, "127.0.0.1", 25565);
	game.loadScene<PauseScene>(Scene::physics, boxScene);
	game.loadPacketHandler<PHBoxClientNetworkId>(BoxPacket::ClientNetworkIdPacket, boxScene);
	game.loadPacketHandler<PHBoxJoin>(BoxPacket::JoinPacket, boxScene);

	game.host.setConnectCallback(&connectionCallback);
	SuquaLib::RunGame(game);

	return 0;
}
