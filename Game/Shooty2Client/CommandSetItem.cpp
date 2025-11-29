#include "CommandSetItem.h"
#include "../Shooty2Core/InventoryComponent.h"

CommandSetItem::CommandSetItem(ItemSystem& items_, EntityId playerId_, EntityId daemonId_) :
	items{&items_},
	playerId{ playerId_ },
	daemonId{ daemonId_ }
{}

std::string CommandSetItem::getTag() const {
	return "setitem";
}

void CommandSetItem::onCommand(const std::vector<std::string>& args) {
	int slot = std::stoi(args[1]);
	if (slot < 0 || slot > 3) return;

	EntityId e = playerId;
	int trueSlot = slot;
	if (slot > 1) {
		e = daemonId;
		trueSlot -= 2;
	}

	auto* invComp = EntitySystem::GetComp<InventoryComponent>(e);
	if(slot > 1 && args[2] == "item:dash")
		invComp->setActionItem(trueSlot, items->getItem(args[2]), playerId);
	else 
		invComp->setActionItem(trueSlot, items->getItem(args[2]));
}