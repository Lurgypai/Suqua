#include "InventoryItemGFXSystem.h"

void InventoryItemGFXSystem::registerGFX(const std::string& renderTag, InventoryItemGFX::RenderMode renderMode, Vec2f renderOffset) {
	gfx.emplace(renderTag, InventoryItemGFX{ renderTag, renderMode, renderOffset });
}

const InventoryItemGFX& InventoryItemGFXSystem::getGFX(const std::string& renderTag) const {
	auto item = gfx.find(renderTag);
	if (item == gfx.end()) return InventoryItemGFX{ "", InventoryItemGFX::RenderMode::none, Vec2f{} };
	return item->second;
}
