#pragma once
#include "../Stabby2Core/PlayingScene.h"

class ClientPlayingScene : public PlayingScene {
public:
	ClientPlayingScene(SceneId id_, FlagType flags_, InputDeviceId keyboard_);

	virtual void load(Game& game) override;
	virtual void renderStep(Game& game) override;

	EntityId addPlayer() override;
	void addControl(EntityId id);
private:
	InputDeviceId keyboard;

	Framebuffer screenBuffer;
};