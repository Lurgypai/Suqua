#pragma once
#include "Scene.h"
#include "PhysicsSystem.h"
#include "InputDevice.h"
#include "OnlineSystem.h"
#include <vector>

class BoxScene : public Scene {
public:
	BoxScene(SceneId id_, char flags_);
	// Inherited via Scene
	virtual void load(Game& game) override;
	virtual void prePhysicsStep(Game& game) override;
	virtual void physicsStep(Game& game) override;
	virtual void postPhysicsStep(Game& game) override;
	virtual void preRenderStep(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void postRenderStep(Game& game) override;
	virtual void unload(Game& game) override;
	virtual void onConnect(Game& game, PeerId connectingId) override;
	virtual void onDisconnect(Game& game, PeerId disconnectId) override;

	EntityId addBox(Game& game, bool isClient, NetworkId targetId = 0);
private:
	PhysicsSystem physics;
protected:
	std::vector<EntityId> boxes;
};