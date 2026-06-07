#include "HandItemGFXSystem.h"

void HandItemGFXSystem::registerGFX(const std::string& renderTag, HandItemGFX::RenderMode renderMode, Vec2f renderOffset) {
	gfx.emplace(renderTag, HandItemGFX{ renderTag, renderMode, renderOffset });
}

HandItemGFX HandItemGFXSystem::getGFX(const std::string& renderTag) const {
	auto item = gfx.find(renderTag);
	if (item == gfx.end()) return HandItemGFX{ "", HandItemGFX::RenderMode::none, Vec2f{} };
	return item->second;
}
