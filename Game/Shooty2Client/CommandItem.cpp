#include "CommandItem.h"

#include <format>
#include <vector>

#include "DebugIO.h"

#include "../Shooty2Core/InventoryComponent.h"

CommandItem::CommandItem(ItemSystem& items_, EntityId playerId_) :
    items{&items_},
    playerId{playerId_}
{}

std::string CommandItem::getTag() const {
    return "item";
}

void CommandItem::onCommand(const std::vector<std::string>& args) {
    if(args.size() != 3) {
        DebugIO::printLine(std::format(
                    "Incorrect arg count, was {}, 3 expected",
                    args.size()
                    ));
    }
    std::string itemTag = args[1];
    int count = std::stoi(args[2]);

    if(!items->hasItem(itemTag)) {
        DebugIO::printLine(std::format(
                    "Invalid item tag \"{}\"",
                    itemTag
                    ));
    }

    auto* plrInv = EntitySystem::GetComp<InventoryComponent>(playerId);
    plrInv->setItemCount(itemTag, count);
}
