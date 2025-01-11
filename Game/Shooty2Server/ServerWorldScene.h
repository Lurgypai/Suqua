#pragma once
#include "Scene.h"
#include "PhysicsSystem.h"

class ServerWorldScene : public Scene {
public:
	// Inherited via Scene
	ServerWorldScene(SceneId id_, Scene::FlagType flags_);

	virtual void load(Game& game) override;
	virtual void physicsStep(Game& game) override;
	virtual void renderUpdateStep(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void unload(Game& game) override;
	virtual void onConnect(Game& game, PeerId connectingId) override;
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer) override;
private:
	int currPlayerCount;

    EntityId dummy;

    PhysicsSystem physics;
};
