#pragma once

#include <array>

#include "Sprite.h"
#include "RectDrawable.h"
#include "EntitySystem.h"
#include "Controller.h"

#include "../Shooty2Core/ItemSystem.h"

#include "InterfaceItemGFXSystem.h"
#include "MenuTab.h"

struct InventoryMenuItem {
    Sprite sprite;
    RectDrawable outline;
};

class InventoryMenu {
public:
    InventoryMenu(const ItemSystem& itemSys_, const InterfaceItemGFXSystem& itemGfx_);

    void open(MenuTab::InventoryTab tab);
    void close();
    void update(const Controller& cont);
    void render();

    EntityId playerId;
    EntityId daemonId;
private:
    enum class State {
        top
    } state;

    enum class EquipSelected {
        lhand,
        rhand,
        d_lhand,
        d_rhand,
        head,
        chest,
        feet
    };

    void navigateTop(const Controller& cont);
    void navigateEquip(const Controller& cont);

    static constexpr int SHOW_COUNT = 10;

    // item selected
    int curSelected;
    std::string curSelectedTag;
    int curEquipSelected;

    // item at top
    int curMin;
    // number of items to render
    int curShowCount;

    float baseRowOffsetX;
    float baseRowOffsetY;
    float rowOffsetY;

    float equipBaseOffsetX;
    float equipOffsetX;
    float equipBaseOffsetY;
    float equipOffsetY;

    std::array<InventoryMenuItem, SHOW_COUNT> items;
    std::array<InventoryMenuItem, 7> equiped;

    const ItemSystem& itemSys;
    const InterfaceItemGFXSystem& itemGfx;

    bool isOpen;
};
