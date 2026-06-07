#include "InventoryMenu.h"
#include "MenuTab.h"
#include "../Shooty2Core/InventoryComponent.h"
#include "../Shooty2Core/HandComponent.h"

// #include <format>
// #include "DebugIO.h"

using namespace MenuTab;

InventoryMenu::InventoryMenu(const InterfaceItemGFXSystem& itemGfx_) :
    state{State::top},
    curSelected{0},
    curEquipSelected{0},
    curMin{0},
    curShowCount{0},
    baseRowOffsetX{50},
    baseRowOffsetY{125},
    rowOffsetY{25},
    equipBaseOffsetX{550},
    equipOffsetX{25},
    equipBaseOffsetY{350},
    equipOffsetY{25},
    itemGfx{ itemGfx_ },
    isOpen{false}
{
    for(auto& item : items) {
        item.outline.shape = {{}, {20, 20}};
        item.outline.depth = -1.f;
    }

    for(auto& item : equiped) {
        item.outline.shape = {{}, {20, 20}};
        item.outline.depth = -1.f;
    }
}


void InventoryMenu::open(InventoryTab tab) {
    curSelected = 0; 
    curMin = 0;

    InventoryComponent* comp = EntitySystem::GetComp<InventoryComponent>(playerId);

    auto uniquItemCount = comp->getUniqueItemCount();
    curShowCount = uniquItemCount < SHOW_COUNT ? uniquItemCount : SHOW_COUNT;

    isOpen = true;
}

void InventoryMenu::close() {
    isOpen = false;
}

void InventoryMenu::navigateTop(const Controller& cont) {
    InventoryComponent* comp = EntitySystem::GetComp<InventoryComponent>(playerId);
    auto uniquItemCount = comp->getUniqueItemCount();

    // calculate new selection
    if(cont.toggled(ControllerBits::UP) && cont[ControllerBits::UP]) {
        --curSelected;
        if(curSelected < 0) curSelected = 0;
    }
    if(cont.toggled(ControllerBits::DOWN) && cont[ControllerBits::DOWN]) {
        ++curSelected;
        if(curSelected >= uniquItemCount) curSelected = uniquItemCount - 1;
    }

    // move sliding boxes
    if(curSelected < curMin) curMin = curSelected;
    else if (curSelected >= curMin + SHOW_COUNT) curMin = (curSelected - SHOW_COUNT) + 1;

    // select an item
    if(cont.toggled(ControllerBits::BUTTON_1) && cont[ControllerBits::BUTTON_1]) state = State::equip;
}

void InventoryMenu::navigateEquip(const Controller& cont) {

    if(cont.toggled(ControllerBits::LEFT) && cont[ControllerBits::LEFT]) {
        --curEquipSelected;
        if(curEquipSelected < 0) curEquipSelected = 0;
    }
    if(cont.toggled(ControllerBits::RIGHT) && cont[ControllerBits::RIGHT]) {
        ++curEquipSelected;
        if(curEquipSelected >= 7) curEquipSelected = 6;
    }

    if(cont.toggled(ControllerBits::BUTTON_2) && cont[ControllerBits::BUTTON_2]) state = State::top;
}

void InventoryMenu::update(const Controller& cont) {
    if(!isOpen) return;

    switch(state) {
        case State::top:
            navigateTop(cont);
            break;
        case State::equip:
            navigateEquip(cont);
            break;
    }
}

void InventoryMenu::render() {
    if(!isOpen) return;
    int curItemIdx = 0;
    InventoryComponent* comp = EntitySystem::GetComp<InventoryComponent>(playerId);
    HandComponent* hands = EntitySystem::GetComp<HandComponent>(playerId);
    for(const auto& pair : comp->items) {
        if(curItemIdx < curMin) {
            ++curItemIdx;
            continue;
        }

        int curRenderIdx = curItemIdx - curMin;
        if(curRenderIdx == curShowCount) break;

        auto& curMenuItem = items[curRenderIdx];
        const InterfaceItemGFX& gfx = itemGfx.getGFX(pair.first);
        curMenuItem.sprite.loadTexture(gfx.smallTag);
        
        ++curItemIdx;
    }

    for(int curRenderIdx = 0; curRenderIdx != curShowCount; ++curRenderIdx) {
        auto& item = items[curRenderIdx];

        // calculate pos
        Vec2f boxPos{baseRowOffsetX, baseRowOffsetY + rowOffsetY * curRenderIdx};

        // set pos
        item.outline.setPos(boxPos);
        item.sprite.setPos(boxPos + Vec2f{2.f, 2.f});
        
        // set color
        if(curRenderIdx + curMin != curSelected) item.outline.c = Color{1.f, 1.f, 1.f, 1.f};
        else {
            if(state == State::top) item.outline.c = Color{1.f, 0.f, 0.f, 1.f};
            else item.outline.c = Color{1.f, 1.f, 1.f, 1.f};
        }

        // draw
        item.sprite.draw();
        item.outline.draw();
    }

    for(int curPlrHand = 0; curPlrHand != 2; ++curPlrHand) {
        int equipRenderIdx = curPlrHand;
        const std::string& tag = hands->getHandTag(curPlrHand);
        const InterfaceItemGFX& gfx = itemGfx.getGFX(tag);
        equiped[equipRenderIdx].sprite.loadTexture(gfx.smallTag);
    }

    for(int curRenderIdx = 0; curRenderIdx != equiped.size(); ++curRenderIdx) {
        auto& item = equiped[curRenderIdx];

        int x = curRenderIdx % 4;
        int y = curRenderIdx / 4;
        // calculate pos
        Vec2f boxPos{equipBaseOffsetX + x * equipOffsetX, equipBaseOffsetY + y * equipOffsetY};

        // set pos
        item.outline.setPos(boxPos);
        item.sprite.setPos(boxPos + Vec2f{2.f, 2.f});
        
        // set color
        if(curRenderIdx != curEquipSelected) item.outline.c = Color{1.f, 1.f, 1.f, 1.f};
        else {
            if(state == State::equip) item.outline.c = Color{1.f, 0.f, 0.f, 1.f};
            else item.outline.c = Color{1.f, 1.f, 1.f, 1.f};
        }

        // draw
        item.sprite.draw();
        item.outline.draw();
    }
}
