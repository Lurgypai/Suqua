#pragma once
#include "Scene.h"
#include "PhysicsSystem.h"
#include "CombatSystem.h"

class WorldScene : public Scene {
public:
	WorldScene(SceneId id_, Scene::FlagType flags_);
	// Inherited via Scene
	virtual void load(Game& game) override;
	virtual void physicsStep(Game& game) override;
	virtual void renderUpdateStep(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void unload(Game& game) override;
	virtual void onConnect(Game& game, PeerId connectingId) override;
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer) override;

private:
	// player owned by this client
	EntityId myPlayerId;

	InputDeviceId playerInput;
	PhysicsSystem physics;
	CombatSystem combat;

	Framebuffer screenBuffer;
};