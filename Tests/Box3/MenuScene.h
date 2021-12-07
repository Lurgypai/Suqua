#pragma once
#include "Scene.h"
#include <array>

class MenuScene : public Scene {
public:
	MenuScene(SceneId id_, FlagType flags_, InputDeviceId input);
	virtual void load(Game& game);
	virtual void prePhysicsStep(Game& game);
	virtual void physicsStep(Game& game);
	virtual void postPhysicsStep(Game& game);

	virtual void preRenderStep(Game& game);
	virtual void renderStep(Game& game);
	virtual void postRenderStep(Game& game);
	virtual void unload(Game& game);

	virtual void onConnect(Game& game, PeerId connectingId);
private:
	InputDeviceId input;
};
