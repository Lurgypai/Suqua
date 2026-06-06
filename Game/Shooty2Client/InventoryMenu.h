#pragma once

#include <array>

#include "Sprite.h"
#include "EntitySystem.h"

#include "MenuTab.h"

struct InventoryMenuItem {
    Sprite sprite;
};

class InventoryMenu {
public:
    InventoryMenu(EntityId playerId_);

    void open(MenuTab::InventoryTab tab);
    void close();
    void update();
    void render();
private:
    static constexpr int SHOW_COUNT = 5;

    EntityId playerId;

    // item selected
    int curSelected;
    // item at top
    int curMin;
    // number of items to render
    int cur_show_count;

    std::array<InventoryMenuItem, SHOW_COUNT> items;
};
