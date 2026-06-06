#pragma once
#include "Scene.h"

#include "Sprite.h"

class MenuScene : public Scene {
public:
	MenuScene(SceneId id_, Scene::FlagType flags_, SceneId playingScene_, InputDeviceId input_);
	// Inherited via Scene
	virtual void load(Game& game) override;
	virtual void physicsStep(Game& game) override;
	virtual void renderUpdateStep(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void unload(Game& game) override;
	virtual void onConnect(Game& game, PeerId connectingId) override;
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer) override;
private:
    SceneId playingScene;
    InputDeviceId input;

    int superTab;
    int subTab;

    int maxSuperTab;
    int maxSubTab;

    // initial offset
    int superTabBaseOffset;
    // multiples of this applied to calculate correct pos
    int superTabOffset;

    int subTabBaseOffset;
    int subTabOffset;

    Sprite superTabBox;
    Sprite subTabBox;
    Sprite background;

    Framebuffer screenBuffer;
};
