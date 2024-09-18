#pragma once
#include "Scene.h"

// Scenes are used to manage entities
// A game consists of a series of "Scene"s which contain entities
// A game might for instance, have a "WorldScene" containing the player and the tiles
// and a "PauseScene" enabled when the game is paused.
class WorldScene : public Scene {
public:
	WorldScene(SceneId id_, Scene::FlagType flags_);
	// Inherited via Scene
    // when the scene is started
	virtual void load(Game& game) override;
    // physics update, called at the physics tick rate
	virtual void physicsStep(Game& game) override;
    // render update, called at the physics tick rate, used to synchronize whatever needs to be drawn with its true state
	virtual void renderUpdateStep(Game& game) override;
    // render step, for drawing everything
	virtual void renderStep(Game& game) override;
    // unload, if the scene will be closed at some point and cleanup is needed
	virtual void unload(Game& game) override;

    //these are called when peers connect to this game instance. They mostly aren't used in clients
	virtual void onConnect(Game& game, PeerId connectingId) override;
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer) override;

private:
	Framebuffer screenBuffer;
};
