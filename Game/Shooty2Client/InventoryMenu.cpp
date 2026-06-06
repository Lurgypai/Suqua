#include "InventoryMenu.h"
#include "MenuTab.h"
#include "../Shooty2Core/InventoryComponent.h"

using namespace MenuTab;

InventoryMenu::InventoryMenu(EntityId playerId_)
    : playerId{playerId_}
{}


void InventoryMenu::open(InventoryTab tab) {
    InventoryComponent* comp = EntitySystem::GetComp<InventoryComponent>(playerId);
}

void InventoryMenu::close() {
}

void InventoryMenu::update() {}

void InventoryMenu::render() {}
