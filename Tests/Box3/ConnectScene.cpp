#include "ConnectScene.h"
#include "Game.h"

ConnectScene::ConnectScene(SceneId id_, FlagType flags_, const std::string& ip_, int port_) :
	Scene{id_, flags_},
	ip{ip_},
	port{port_}
{
}

void ConnectScene::load(Game& game)
{
	game.host.tryConnect(ip, port, 10);
}

void ConnectScene::prePhysicsStep(Game& game)
{
}

void ConnectScene::physicsStep(Game& game)
{
}

void ConnectScene::postPhysicsStep(Game& game)
{
}

void ConnectScene::preRenderStep(Game& game)
{
}

void ConnectScene::renderStep(Game& game)
{
}

void ConnectScene::postRenderStep(Game& game)
{
}

void ConnectScene::unload(Game& game)
{
}
