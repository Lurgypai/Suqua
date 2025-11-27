#include "InventoryComponent.h"
#include "ControllerComponent.h"

InventoryComponent::InventoryComponent(EntityId id_) :
	id{ id_ },
	actionItems{},
	storageItems{}
{
	actionItems.resize(4);
}

void InventoryComponent::update(const ItemSystem& itemSystem) {
	/* 11 = mouse 1
	* 12 = mouse 2
	* 6 = A
	* 5 = Space
	*/

	auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);
	if (contComp == nullptr) return;

	const auto& controller = contComp->getController();
	if (controller[ControllerBits::BUTTON_11]) {
		if(!actionItems[0].tag.empty()) itemSystem.useItem(actionItems[0].tag);
	}
	else if (controller[ControllerBits::BUTTON_12]) {
		if(!actionItems[1].tag.empty()) itemSystem.useItem(actionItems[1].tag);
	}
	else if (controller[ControllerBits::BUTTON_6]) {
		if(!actionItems[2].tag.empty()) itemSystem.useItem(actionItems[2].tag);
	}
	else if (controller[ControllerBits::BUTTON_5]) {
		if(!actionItems[3].tag.empty()) itemSystem.useItem(actionItems[3].tag);
	}
}

void InventoryComponent::setActionItem(int slot, const std::string& tag) {
	// maybe crash?
	if (slot < 0 && slot > 3) return;

	actionItems[slot] = InventoryItem{ tag, 1 };
}
