#include "CommandSetItem.h"
#include "DebugIO.h"
#include "../Shooty2Core/HandComponent.h"

CommandSetItem::CommandSetItem(ItemSystem& items_, EntityId playerId_, EntityId daemonId_) :
	items{&items_},
	playerId{ playerId_ },
	daemonId{ daemonId_ }
{}

std::string CommandSetItem::getTag() const {
	return "setitem";
}

void CommandSetItem::onCommand(const std::vector<std::string>& args) {
	if (args.size() != 3) {
		DebugIO::printLine("Error: Incorrect arg count.");
		return;
	}

	int slot = 0;
	try {
		slot = std::stoi(args[1]);
	}
	catch (const std::exception&) {
		DebugIO::printLine("Error: Unable to parse slot number.");
		return;
	}

	if (slot < 0 || slot > 3) {
		DebugIO::printLine("Error: Slot number out of range (0-3).");
		return;
	}

	if (!items->hasItem(args[2])) {
		DebugIO::printLine("Error: Invalid item.");
		return;
	}

	EntityId e = playerId;
	int trueSlot = slot;
	if (slot > 1) {
		e = daemonId;
		trueSlot -= 2;
	}

	auto* invComp = EntitySystem::GetComp<HandComponent>(e);
	if(slot > 1)
		invComp->setItem(trueSlot, items->getItem(args[2]), playerId);
	else 
		invComp->setItem(trueSlot, items->getItem(args[2]));
}
