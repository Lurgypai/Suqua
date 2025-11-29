#pragma once
#include <string>
#include <unordered_map>

#include "Vec2.h"


struct InventoryItemGFX {
	enum RenderMode {
		sprite,
		particle,
		none
	};

	std::string renderTag;
	RenderMode renderMode;
	Vec2f renderOffset;
};

class InventoryItemGFXSystem {
public:
	void registerGFX(const std::string& renderTag, InventoryItemGFX::RenderMode renderMode, Vec2f renderOffset);
	const InventoryItemGFX& getGFX(const std::string& renderTag) const;
private:
	std::unordered_map<std::string, InventoryItemGFX> gfx;
};