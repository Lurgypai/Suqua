#include "MenuScene.h"
#include "NetworkDataComponent.h"
#include "ButtonComponent.h"
#include "PositionData.h"
#include "Game.h"

MenuScene::MenuScene(SceneId id_, FlagType flags_, InputDeviceId input_) : Scene{ id_, flags_ }, input{ input_ } {}

void MenuScene::load(Game& game)
{
	auto uiEntities = addEntities(9);

	GLRenderer::LoadTexture("ui_interp.png", "ui.interp");
	GLRenderer::LoadTexture("ui_predict.png", "ui.predict");
	GLRenderer::LoadTexture("ui_reconcile.png", "ui.reconcile");
	GLRenderer::LoadTexture("ui_check_circle.png", "ui.check_circle");
	GLRenderer::LoadTexture("ui_checkmark.png", "ui.checkmark");

	EntitySystem::MakeComps<RenderComponent>(uiEntities.size(), uiEntities.data());
	EntitySystem::MakeComps<ButtonComponent>(3, uiEntities.data());
	addEntityInputs({ {uiEntities[0], input}, {uiEntities[1], input}, {uiEntities[2], input} });
	Vec2f offset = { 20, 30 };
	float margin = 50;
	for (int i = 0; i != 3; ++i) {
		auto button = uiEntities[i];
		auto* render = EntitySystem::GetComp<RenderComponent>(button);
		render->loadDrawable<Sprite>("ui.check_circle");
		render->getDrawable<Sprite>()->setScale({ 0.5, 0.5 });

		auto* data = EntitySystem::GetComp<NetworkDataComponent>(button);
		data->get<float>(X) = offset.x;
		data->get<float>(Y) = offset.y;
		offset.y += margin + render->getDrawable<Sprite>()->getObjRes().y * render->getDrawable<Sprite>()->getScale().y;
	}

	auto uiPredict = uiEntities[3];
	auto* predictRender = EntitySystem::GetComp<RenderComponent>(uiPredict);
	predictRender->loadDrawable<Sprite>("ui.predict");
	auto* predictData = EntitySystem::GetComp<NetworkDataComponent>(uiPredict);
	predictData->get<float>(X) = 70;
	predictData->get<float>(Y) = 5;

	auto uiReconcile = uiEntities[4];
	auto* reconcileRender = EntitySystem::GetComp<RenderComponent>(uiReconcile);
	reconcileRender->loadDrawable<Sprite>("ui.reconcile");
	reconcileRender->getDrawable<Sprite>()->setScale({ 1.2, 1.2 });
	auto* reconcileData = EntitySystem::GetComp<NetworkDataComponent>(uiReconcile);
	reconcileData->get<float>(X) = 70;
	reconcileData->get<float>(Y) = 80;

	auto uiInterp = uiEntities[5];
	auto* interpRender = EntitySystem::GetComp<RenderComponent>(uiInterp);
	interpRender->loadDrawable<Sprite>("ui.interp");
	interpRender->getDrawable<Sprite>()->setScale({ 1.2, 1.2 });
	auto* interpData = EntitySystem::GetComp<NetworkDataComponent>(uiInterp);
	interpData->get<float>(X) = 70;
	interpData->get<float>(Y) = 160;

	Camera cam{ {0,0}, {1920, 1080}, 1.0 };
	camId = GLRenderer::addCamera(cam);

	for (int i = 6; i != 9; ++i) {
		auto check = uiEntities[i];
		auto* render = EntitySystem::GetComp<RenderComponent>(check);
		render->loadDrawable<Sprite>("ui.checkmark");
		render->getDrawable<Sprite>()->setScale({ 0.5, 0.5 });

		auto* data = EntitySystem::GetComp<NetworkDataComponent>(check);
		data->get<float>(X) = -50;
		data->get<float>(Y) = -50;
		offset.y += margin + render->getDrawable<Sprite>()->getObjRes().y * render->getDrawable<Sprite>()->getScale().y;
	}
}

void MenuScene::prePhysicsStep(Game& game)
{
	std::vector<EntityId> entities{ getEntities().begin(), getEntities().end() };
	for (int i = 0; i != 3; ++i) {
		EntitySystem::GetComp<ButtonComponent>(entities[i])->update();
	}
	if (EntitySystem::GetComp<ButtonComponent>(entities[1])->state) EntitySystem::GetComp<ButtonComponent>(entities[0])->state = true;

	if (EntitySystem::GetComp<ButtonComponent>(entities[1])->state) game.inputLagCompensationLevel = 2;
	else if (EntitySystem::GetComp<ButtonComponent>(entities[0])->state) game.inputLagCompensationLevel = 1;
	else game.inputLagCompensationLevel = 0;

	game.sync.doInterp = EntitySystem::GetComp<ButtonComponent>(entities[2])->state;
}

void MenuScene::physicsStep(Game& game) {


}

void MenuScene::postPhysicsStep(Game& game)
{
}

void MenuScene::preRenderStep(Game& game)
{
}

void MenuScene::renderStep(Game& game) {
	std::vector<EntityId> entities{ getEntities().begin(), getEntities().end() };

	Vec2f checkmarkOffset{ 0, -30 };
	for (int i = 0; i != 3; ++i) {
		Vec2f checkPos = { -50, -50 };
		if (EntitySystem::GetComp<ButtonComponent>(entities[i])->state) {
			auto* data = EntitySystem::GetComp<NetworkDataComponent>(entities[i]);
			checkPos = { data->get<float>(X), data->get<float>(Y) };
		}
		checkPos += checkmarkOffset;
		auto* checkmarkData = EntitySystem::GetComp<NetworkDataComponent>(entities[i + 6]);
		checkmarkData->get<float>(X) = checkPos.x;
		checkmarkData->get<float>(Y) = checkPos.y;
	}
	drawScene(game.getRender());
}

void MenuScene::postRenderStep(Game& game)
{
}

void MenuScene::unload(Game& game)
{
}

void MenuScene::onConnect(Game& game, PeerId connectingId)
{
}
