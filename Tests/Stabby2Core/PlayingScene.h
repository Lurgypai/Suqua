#pragma once
#include "Scene.h"
#include "PhysicsSystem.h"
#include "CombatSystem.h"

class PlayingScene : public Scene {
public:
	PlayingScene(SceneId id_, FlagType flags_);
	virtual ~PlayingScene();

	virtual void load(Game& game);
	virtual void prePhysicsStep(Game& game);
	virtual void physicsStep(Game& game);
	virtual void postPhysicsStep(Game& game);

	virtual void preRenderStep(Game& game);
	virtual void renderStep(Game& game);
	virtual void postRenderStep(Game& game);
	virtual void unload(Game& game);

	virtual void onConnect(Game& game, PeerId connectingId);
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer);

	virtual EntityId addPlayer();
protected:
	CombatSystem combat;
	PhysicsSystem physics;
	EntityId platformId;
};