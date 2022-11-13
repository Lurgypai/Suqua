#pragma once
#include "Scene.h"
#include "PhysicsSystem.h"
#include "CombatSystem.h"
#include "RespawnSystem.h"

class PlayingScene : public Scene {
public:
	PlayingScene(SceneId id_, FlagType flags_);
	virtual ~PlayingScene();

	virtual void load(Game& game) override;
	virtual void physicsStep(Game& game) override;

	virtual void renderUpdateStep(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void unload(Game& game) override;

	virtual void onConnect(Game& game, PeerId connectingId) override;
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer) override;

	virtual EntityId addPlayer();
	virtual EntityId addZombie(Game& game, const Vec2f& targetPos);
protected:
	CombatSystem combat;
	PhysicsSystem physics;
	RespawnSystem respawn;
	EntityId platformId;
};