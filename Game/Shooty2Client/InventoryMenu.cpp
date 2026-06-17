#include "InventoryMenu.h"
#include "MenuTab.h"
#include "../Shooty2Core/InventoryComponent.h"
#include "../Shooty2Core/HandComponent.h"

#include <format>
#include "DebugIO.h"

using namespace MenuTab;

InventoryMenu::InventoryMenu(const ItemSystem& itemSys_, const InterfaceItemGFXSystem& itemGfx_) :
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
    isOpen{false},
    itemSys{itemSys_}
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
    state = State::top;

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

    // apply selected item
    if(cont.toggled(ControllerBits::BUTTON_1) && cont[ControllerBits::BUTTON_1]) {
        if(!itemSys.hasItem(curSelectedTag)) {
            DebugIO::printLine(std::format("WARN: No item with tag \"{}\" found.", curSelectedTag));
        }
        else {
            InventoryComponent* plrInv = EntitySystem::GetComp<InventoryComponent>(playerId);
            HandComponent* plrHands = EntitySystem::GetComp<HandComponent>(playerId);
            HandComponent* daemonHands = EntitySystem::GetComp<HandComponent>(daemonId);
            EquipSelected selected = static_cast<EquipSelected>(curEquipSelected);
            switch(selected) {
                case EquipSelected::lhand:
                    plrHands->setItem(0, itemSys.getItem(curSelectedTag));
                    break;
                case EquipSelected::rhand:
                    plrHands->setItem(1, itemSys.getItem(curSelectedTag));
                    break;
                case EquipSelected::d_lhand:
                    daemonHands->setItem(0, itemSys.getItem(curSelectedTag), playerId);
                    break;
                case EquipSelected::d_rhand:
                    daemonHands->setItem(1, itemSys.getItem(curSelectedTag), playerId);
                    break;
                case EquipSelected::head:
                    break;
                case EquipSelected::chest:
                    break;
                case EquipSelected::feet:
                    break;
            }

            // scan all hands and remove if necessary
            int countInInv = plrInv->getItemCount(curSelectedTag);
            int countEquiped = 0;
            for(int handSlot = 3; handSlot >= 0; --handSlot) {
                // select plr or daemon
                HandComponent* curHands = nullptr;
                if(handSlot > 1) curHands = daemonHands;
                else curHands = plrHands;

                // don't remove the one we placed, check if has item
                if(handSlot != curEquipSelected && curHands->getHandTag(handSlot % 2) == curSelectedTag) ++countEquiped;
                
                // if to many, remove, and exit 
                if(countEquiped >= countInInv) {
                    curHands->clearItem(handSlot % 2);
                    break;
                }
            }
        }
    }

    if(cont.toggled(ControllerBits::BUTTON_2) && cont[ControllerBits::BUTTON_2]) {
        HandComponent* plrHands = EntitySystem::GetComp<HandComponent>(playerId);
        HandComponent* daemonHands = EntitySystem::GetComp<HandComponent>(daemonId);
        EquipSelected selected = static_cast<EquipSelected>(curEquipSelected);
        switch(selected) {
            case EquipSelected::lhand:
                plrHands->clearItem(0);
                break;
            case EquipSelected::rhand:
                plrHands->clearItem(1);
                break;
            case EquipSelected::d_lhand:
                daemonHands->clearItem(0);
                break;
            case EquipSelected::d_rhand:
                daemonHands->clearItem(1);
                break;
            case EquipSelected::head:
                break;
            case EquipSelected::chest:
                break;
            case EquipSelected::feet:
                break;
        }
    }
}

void InventoryMenu::update(const Controller& cont) {
    if(!isOpen) return;

    switch(state) {
        case State::top:
            navigateTop(cont);
            navigateEquip(cont);
            break;
    }
}

void InventoryMenu::render() {
    if(!isOpen) return;
    int curItemIdx = 0;
    InventoryComponent* comp = EntitySystem::GetComp<InventoryComponent>(playerId);
    HandComponent* hands = EntitySystem::GetComp<HandComponent>(playerId);
    HandComponent* daemonHands = EntitySystem::GetComp<HandComponent>(daemonId);
    for(const auto& pair : comp->items) {
        if(curItemIdx < curMin) {
            ++curItemIdx;
            continue;
        }

        int curRenderIdx = curItemIdx - curMin;
        if(curRenderIdx == curShowCount) break;

        if(curItemIdx == curSelected) curSelectedTag = pair.first;

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
        else item.outline.c = Color{1.f, 0.f, 0.f, 1.f};

        // draw
        item.sprite.draw();
        item.outline.draw();
    }

    for(int curPlrHand = 0; curPlrHand != 2; ++curPlrHand) {
        int equipRenderIdx = curPlrHand;
        if(!hands->handIsActive(curPlrHand)) {
            equiped[equipRenderIdx].sprite.loadTexture("none");
            continue;
        }

        const std::string& tag = hands->getHandTag(curPlrHand);
        const InterfaceItemGFX& gfx = itemGfx.getGFX(tag);
        equiped[equipRenderIdx].sprite.loadTexture(gfx.smallTag);
    }
    for(int curDaemonHand = 0; curDaemonHand != 2; ++curDaemonHand) {
        int equipRenderIdx = curDaemonHand + 2;
        if(!daemonHands->handIsActive(curDaemonHand)) {
            equiped[equipRenderIdx].sprite.loadTexture("none");
            continue;
        }

        const std::string& tag = daemonHands->getHandTag(curDaemonHand);
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
        else item.outline.c = Color{1.f, 0.f, 0.f, 1.f};

        // draw
        if(item.sprite.texture_tag != "none") item.sprite.draw();
        item.outline.draw();
    }
}
