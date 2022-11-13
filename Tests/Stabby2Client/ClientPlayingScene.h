#pragma once
#include "../Stabby2Core/PlayingScene.h"

class ClientPlayingScene : public PlayingScene {
public:
	ClientPlayingScene(SceneId id_, FlagType flags_, InputDeviceId keyboard_);

	virtual void load(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void renderUpdateStep(Game& game) override;
	virtual void physicsStep(Game& game) override;

	EntityId addPlayer() override;
	EntityId addZombie(Game& game, const Vec2f& targetPos) override;
	void addControl(EntityId id);
private:
	EntityId player;
	InputDeviceId keyboard;

	Framebuffer screenBuffer;
};