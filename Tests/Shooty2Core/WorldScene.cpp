#include "WorldScene.h"
#include "PlayerComponent.h"
#include "PhysicsComponent.h"
#include "Game.h"

void WorldScene::load(Game& game)
{
}

void WorldScene::physicsStep(Game& game)
{
}

void WorldScene::renderUpdateStep(Game& game)
{
}

void WorldScene::renderStep(Game& game)
{
	drawScene(game.getRender());
}

void WorldScene::unload(Game& game)
{
}

void WorldScene::onConnect(Game& game, PeerId connectingId)
{
}

void WorldScene::onDisconnect(Game& game, PeerId disconnectedPeer)
{
}
