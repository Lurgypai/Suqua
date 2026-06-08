#pragma once
#include "Scene.h"

#include <string>
#include <array>

#include "Sprite.h"
#include "TextDrawable.h"
#include "InterfaceItemGFXSystem.h"
#include "InventoryMenu.h"

class MenuScene : public Scene {
public:
	MenuScene(SceneId id_, Scene::FlagType flags_, SceneId playingScene_, InputDeviceId input_,
            const ItemSystem& itemSys_, const InterfaceItemGFXSystem& itemGfx_,
            EntityId playerId, EntityId daemonId_);
	// Inherited via Scene
	virtual void load(Game& game) override;
	virtual void physicsStep(Game& game) override;
	virtual void renderUpdateStep(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void unload(Game& game) override;
	virtual void onConnect(Game& game, PeerId connectingId) override;
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer) override;

private:
    EntityId playerId;
    EntityId daemonId;
    void updateTabs();

    InventoryMenu inventoryMenu;

    SceneId playingScene;
    InputDeviceId input;

    int superTab;
    int subTab;
    int prevSuperTab;
    int prevSubTab;

    int maxSuperTab;
    int maxSubTab;

    // initial offset
    int superTabBaseOffset;
    // multiples of this applied to calculate correct pos
    int superTabOffset;

    int subTabBaseOffset;
    int subTabOffset;

    std::array<std::string, 5> superTabNames;
    std::array<std::array<std::string, 5>, 5> subTabNames;

    std::array<TextDrawable, 5> superTabText;
    std::array<TextDrawable, 5> subTabText;

    Sprite superTabBox;
    Sprite subTabBox;
    Sprite background;

    Framebuffer screenBuffer;
};
