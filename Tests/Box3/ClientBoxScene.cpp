#include "ClientBoxScene.h"
#include "Game.h"

ClientBoxScene::ClientBoxScene(SceneId id_, FlagType flags_, InputDeviceId input_) :
	BoxScene{id_, flags_},
	input{input_}
{}

void ClientBoxScene::load(Game& game)
{

	GLRenderer::LoadTexture("box.png", "box");

	BoxScene::load(game);

	EntitySystem::MakeComps<RenderComponent>(1, &boxId);
	RenderComponent* render = EntitySystem::GetComp<RenderComponent>(boxId);
	render->setDrawable<Sprite>("box");
	render->getDrawable<Sprite>()->setScale({ 3, 3 });

	Camera cam{ {0,0}, {1920, 1080}, 1.0 };
	camId = GLRenderer::addCamera(cam);

	addEntityInputs({ {boxId, input} });
}

void ClientBoxScene::renderStep(Game& game) {
	GLRenderer::Clear();
	drawScene(game.getRender());
	GLRenderer::Swap();
}