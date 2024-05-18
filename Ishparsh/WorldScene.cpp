#include "WorldScene.h"
#include "Game.h"
#include "Sprite.h"
#include "Updater.h"
#include "DebugIO.h"
#include "EntitySystem.h"
#include "RenderComponent.h"
#include "PositionComponent.h"



constexpr unsigned int ScreenWidth = 1920;
constexpr unsigned int ScreenHeight = 1080;

WorldScene::WorldScene(SceneId id_, Scene::FlagType flags_) :
	Scene{ id_, flags_ }
{}

void WorldScene::load(Game& game)
{
	// camera
	Camera cam{ {0, 0}, {ScreenWidth, ScreenHeight}, 1.0f };
	camId = GLRenderer::addCamera(cam);

	// textures
	GLRenderer::LoadTexture("ishparsh_face.png", "ishparsh");

    EntityId ishparsh = addEntities(1)[0];
    EntitySystem::MakeComps<RenderComponent>(1, &ishparsh);
    auto* renderable = EntitySystem::GetComp<RenderComponent>(ishparsh);
    renderable->loadDrawable<Sprite>("ishparsh");

    auto* pos = EntitySystem::GetComp<PositionComponent>(ishparsh);
    pos->setPos({1920 / 2, 1080 / 2});
}

void WorldScene::physicsStep(Game& game)
{
}

void WorldScene::renderUpdateStep(Game& game)
{
}

void WorldScene::renderStep(Game& game)
{
	GLRenderer::Clear();
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
