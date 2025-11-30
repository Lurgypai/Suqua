#pragma once
#include "ComponentMacros.h"
#include <vector>
#include <string>
#include "InventoryItemGFXSystem.h"

struct RenderItem {
	size_t spriteIndex;
	std::string renderTag;
};

class InventoryItemGFXComponent {
    CompMembers(InventoryItemGFXComponent);
public:
	InventoryItemGFXComponent(EntityId id_);

	void update(const InventoryItemGFXSystem& invItemGFXSys);
private:
	std::vector<RenderItem> items;
};
