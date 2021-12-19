#include "ClientPlayingScene.h"
#include "Game.h"
#include "PlayerGFXComponent.h"

ClientPlayingScene::ClientPlayingScene(SceneId id_, FlagType flags_, InputDeviceId keyboard_) :
	PlayingScene{id_, flags_},
	keyboard{keyboard_}
{
}

void ClientPlayingScene::load(Game& game) {
	PlayingScene::load(game);

	GLRenderer::LoadTexture("stabbyman.png", "stabbyman");

	Camera cam{ { 0, 0,}, { 480, 270}, 1.0f};
	camId = GLRenderer::addCamera(cam);

	screenBuffer.bind();
	screenBuffer.addTexture2D(480, 270, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	screenBuffer.makeDepthBuffer(480, 270);
	screenBuffer.finalizeFramebuffer();
	Framebuffer::unbind();
}

void ClientPlayingScene::renderStep(Game& game) {
	for (auto& gfx : EntitySystem::GetPool<PlayerGFXComponent>()) {
		gfx.update(game.RENDER_STEP * 1000);
	}

	screenBuffer.bind();
	GLRenderer::Clear();
	drawScene(game.getRender());
	Framebuffer::unbind();
	GLRenderer::DrawOverScreen(screenBuffer.getTexture(0).id);
}

void ClientPlayingScene::addPlayer() {
	EntityId playerId = PlayingScene::addPlayer();

	EntitySystem::MakeComps<PlayerGFXComponent>(1, &playerId);

	addEntityInputs({ {playerId, keyboard} });
}
