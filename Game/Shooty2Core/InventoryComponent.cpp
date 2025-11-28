#include "InventoryComponent.h"
#include "ControllerComponent.h"

InventoryComponent::InventoryComponent(EntityId id_) :
	id{ id_ },
	lHandFlag{ControllerBits::BUTTON_11},
	rHandFlag{ControllerBits::BUTTON_12},
	actionItems{},
	storageItems{}
{
	actionItems.resize(SLOT_COUNT);
}

void InventoryComponent::update(Scene& scene, float delta) {
	// update items
	for (auto& item : actionItems) {
		if (item.ability != nullptr) item.ability->update(delta);
	}

	// use items
	auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);
	if (contComp == nullptr) return;

	const auto& controller = contComp->getController();
	int targetItem = -1;
	if (controller[lHandFlag]) {
		targetItem = 0;
	}
	else if (controller[rHandFlag]) {
		targetItem = 1;
	}

	if (targetItem < 0) return;
	if (actionItems[targetItem].ability == nullptr) return;

	actionItems[targetItem].ability->doAbility(scene, id, controller, actionItems[targetItem]);
}

void InventoryComponent::setActionItem(int slot, const Item& item) {
	// maybe crash?
	if (slot < 0 && slot > SLOT_COUNT - 1) return;

	actionItems[slot] = InventoryItem{ item.tag, 1, item.getAbility()};
}
