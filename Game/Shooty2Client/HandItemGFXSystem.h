#pragma once
#include <string>
#include <unordered_map>

#include "../Shooty2Core/ItemSystem.h"
#include "Vec2.h"

// relies on item tags and texture tags being the same, maybe change later

struct HandItemGFX {
	enum RenderMode {
		sprite,
		particle,
		none
	};

	std::string renderTag;
	RenderMode renderMode;
	Vec2f renderOffset;
};

class HandItemGFXSystem {
public:
    void loadGfx(const ItemSystem& items);
	HandItemGFX getGFX(const std::string& itemTag) const;
private:
	std::unordered_map<std::string, HandItemGFX> gfx;
};
