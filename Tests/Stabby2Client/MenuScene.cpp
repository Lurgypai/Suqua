#include "MenuScene.h"
#include "GLRenderer.h"
#include "Game.h"

MenuScene::MenuScene(SceneId id_, FlagType flags_, SceneId playingSceneId_, const std::string& ip_) :
	Scene{id_, flags_},
	menu{"none"},
	playingSceneId{playingSceneId_},
	ip{ ip_ }
{}

void MenuScene::load(Game& game) {
	GLRenderer::LoadTexture("menu.png", "menu");
	menu = Sprite{ "menu" };
	menu.setPos({ 0, 0 });

	Camera cam{ { 0, 0,}, { 480, 270}, 1.0f };
	camId = GLRenderer::addCamera(cam);

	screenBuffer.bind();
	screenBuffer.addTexture2D(480, 270, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	screenBuffer.makeDepthBuffer(480, 270);
	screenBuffer.finalizeFramebuffer();
	Framebuffer::unbind();
}

void MenuScene::prePhysicsStep(Game& game)
{
}

void MenuScene::physicsStep(Game& game) {
	Vec2i mousePos;
	auto mouseState = SDL_GetMouseState(&mousePos.x, &mousePos.y);
	bool left = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
	if (left) {
		game.host.tryConnect(ip, 25565, 10);
		game.sceneOff(id);
		game.sceneOn(playingSceneId);
	}
}

void MenuScene::postPhysicsStep(Game& game)
{
}

void MenuScene::preRenderStep(Game& game)
{
}

void MenuScene::renderStep(Game& game) {
	screenBuffer.bind();
	GLRenderer::Clear();
	GLRenderer::setCamera(camId);
	menu.draw();
	Framebuffer::unbind();
	GLRenderer::DrawOverScreen(screenBuffer.getTexture(0).id);
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

void MenuScene::onDisconnect(Game& game, PeerId disconnectedPeer)
{
}
