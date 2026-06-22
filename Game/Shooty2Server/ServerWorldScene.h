#pragma once
#include "Scene.h"
#include "PhysicsSystem.h"
#include "World.h"
#include "../Shooty2Core/Director.h"
#include "../Shooty2Core/ItemSystem.h"

class ServerWorldScene : public Scene {
public:
	// Inherited via Scene
	ServerWorldScene(SceneId id_, Scene::FlagType flags_,
            ItemSystem& items_, const std::string& worldFile);

	virtual void load(Game& game) override;
	virtual void physicsStep(Game& game) override;
	virtual void renderUpdateStep(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void unload(Game& game) override;
	virtual void onConnect(Game& game, PeerId connectingId) override;
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer) override;
private:
    PhysicsSystem physics;
    ItemSystem& items;
    World world;
    Director director;

	int currPlayerCount;

    EntityId dummy;

};
