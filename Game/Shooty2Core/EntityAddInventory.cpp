#include "EntityAddInventory.h"
#include "InventoryComponent.h"

void EntityAddInventory(EntityId id, const EntityAddInventoryArgs& args) {
	EntitySystem::MakeComps<InventoryComponent>(1, &id);
	auto* invComp = EntitySystem::GetComp<InventoryComponent>(id);

	for (std::size_t i = 0; i != args.itemTags.size(); ++i) {
		const std::string& tag = args.itemTags[i];
		int count = args.itemCounts[i];
		invComp->setItemCount(tag, count);
	}
}