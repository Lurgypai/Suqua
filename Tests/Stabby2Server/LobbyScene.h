#pragma once
#include "Scene.h"

class LobbyScene : public Scene {
public:
	// Inherited via Scene
	LobbyScene(SceneId id_, Scene::FlagType flags_, SceneId playingScene_, int minPlayerCount_ = 2);

	virtual void load(Game& game) override;
	virtual void prePhysicsStep(Game& game) override;
	virtual void physicsStep(Game& game) override;
	virtual void postPhysicsStep(Game& game) override;
	virtual void preRenderStep(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void postRenderStep(Game& game) override;
	virtual void unload(Game& game) override;
	virtual void onConnect(Game& game, PeerId connectingId) override;
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer) override;
private:
	int minPlayerCount;
	int currPlayerCount;
	SceneId playingScene;
};