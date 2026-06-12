#pragma once
#include "World.h"
        // when we
#include "Scene.h"
#include "PhysicsSystem.h"
#include "../Shooty2Core/ItemSystem.h"
#include "../Shooty2Core/CombatSystem.h"

#include "LevelGFXSystem.h"
#include "HandItemGFXSystem.h"

class ClientWorldScene : public Scene {
public:
	ClientWorldScene(SceneId id_, Scene::FlagType flags_, InputDeviceId input,
            ItemSystem& items, HandItemGFXSystem& invItemGfx,
            const std::string& ldtkFileName);
	// Inherited via Scene
	virtual void load(Game& game) override;
	virtual void physicsStep(Game& game) override;
	virtual void renderUpdateStep(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void unload(Game& game) override;
	virtual void onConnect(Game& game, PeerId connectingId) override;
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer) override;

	EntityId myPlayerId;
	EntityId myDaemonId;
    SceneId menuScene;
private:
    // physics has to be constructed before world as its passed to world
	PhysicsSystem physics;

    World world;
    std::string activeLevel;
	// player owned by this client

	InputDeviceId playerInput;
	CombatSystem combat;
	ItemSystem& items;
	HandItemGFXSystem& invItemGfx;
    LevelGFXSystem levelGfx;

	Framebuffer screenBuffer;
};
