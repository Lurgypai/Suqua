#pragma once
#include "ComponentMacros.h"
#include <vector>
#include <string>
#include "InventoryItemGFXSystem.h"

struct RenderItem {
	size_t spriteIndex;
	std::string renderTag;
};

class GunGFXComponent {
    CompMembers(GunGFXComponent);
public:
	GunGFXComponent(EntityId id_);

	void update(const InventoryItemGFXSystem& invItemGFXSys);
private:
	std::vector<RenderItem> items;
};
