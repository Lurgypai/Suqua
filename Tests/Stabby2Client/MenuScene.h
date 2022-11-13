#pragma once
#include "Scene.h"

class MenuScene : public Scene {
public:
	MenuScene(SceneId id_, FlagType flags_, SceneId playingSceneId_, const std::string& ip_);
	virtual void load(Game& game) override;
	virtual void physicsStep(Game& game) override;
	virtual void renderUpdateStep(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void unload(Game& game) override;
	virtual void onConnect(Game& game, PeerId connectingId) override;
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer) override;
private:
	Sprite menu;
	Framebuffer screenBuffer;
	SceneId playingSceneId;
	std::string ip;
};