#include "PauseScene.h"
#include "Game.h"
#include "SDL.h"

PauseScene::PauseScene(SceneId id_, char flags_, SceneId targetScene_) :
	Scene{id_, flags_},
	targetScene{targetScene_}
{}

void PauseScene::load()
{
}

void PauseScene::prePhysicsStep(Game& game) {

}

void PauseScene::physicsStep(Game& game) {
	static bool prevEsc = false;
	auto keyboard = SDL_GetKeyboardState(NULL);
	bool currEsc = keyboard[SDL_SCANCODE_ESCAPE];
	if (!prevEsc && currEsc) {
		game.toggleSceneFlags(targetScene, Scene::input);
	}

	prevEsc = currEsc;
}

void PauseScene::postPhysicsStep(Game& game)
{
}

void PauseScene::preRenderStep(Game& game)
{
}

void PauseScene::renderStep(Game& game)
{
}

void PauseScene::postRenderStep(Game& game)
{
}

void PauseScene::unload()
{
}
