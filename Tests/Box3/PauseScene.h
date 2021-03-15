#pragma once
#include "Scene.h"

class PauseScene : public Scene {
public:
	PauseScene(SceneId id_, char flags_, SceneId targetScene_);
	// Inherited via Scene
	virtual void load() override;
	virtual void prePhysicsStep(Game& game) override;
	virtual void physicsStep(Game& game) override;
	virtual void postPhysicsStep(Game& game) override;
	virtual void preRenderStep(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void postRenderStep(Game& game) override;
	virtual void unload() override;
private:
	SceneId targetScene;
};