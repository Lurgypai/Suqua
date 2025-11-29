#pragma once
#include "World.h"
#include "Scene.h"
#include "PhysicsSystem.h"
#include "../Shooty2Core/ItemSystem.h"
#include "../Shooty2Core/CombatSystem.h"
#include "InventoryItemGFXSystem.h"

class ClientWorldScene : public Scene {
public:
	ClientWorldScene(SceneId id_, Scene::FlagType flags_);
	// Inherited via Scene
	virtual void load(Game& game) override;
	virtual void physicsStep(Game& game) override;
	virtual void renderUpdateStep(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void unload(Game& game) override;
	virtual void onConnect(Game& game, PeerId connectingId) override;
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer) override;

private:
    World world;
    std::string activeLevel;
	// player owned by this client
	EntityId myPlayerId;
	EntityId myDaemonId;

	InputDeviceId playerInput;
	InputDeviceId gunInput;
	PhysicsSystem physics;
	CombatSystem combat;
	ItemSystem items;
	InventoryItemGFXSystem invItemGfx;

	Framebuffer screenBuffer;
};
