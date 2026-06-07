#pragma once

#include <array>

#include "Sprite.h"
#include "RectDrawable.h"
#include "EntitySystem.h"
#include "Controller.h"

#include "InterfaceItemGFXSystem.h"
#include "MenuTab.h"

struct InventoryMenuItem {
    Sprite sprite;
    RectDrawable outline;
};

class InventoryMenu {
public:
    InventoryMenu(const InterfaceItemGFXSystem& itemGfx_);

    void open(MenuTab::InventoryTab tab);
    void close();
    void update(const Controller& cont);
    void render();

    EntityId playerId;
private:
    enum class State {
        top,
        equip
    } state;

    void navigateTop(const Controller& cont);
    void navigateEquip(const Controller& cont);

    static constexpr int SHOW_COUNT = 10;

    // item selected
    int curSelected;
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

    const InterfaceItemGFXSystem& itemGfx;

    bool isOpen;
};
