#pragma once
#include <string>
#include <unordered_map>

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
	void registerGFX(const std::string& renderTag, HandItemGFX::RenderMode renderMode, Vec2f renderOffset);
	HandItemGFX getGFX(const std::string& renderTag) const;
private:
	std::unordered_map<std::string, HandItemGFX> gfx;
};
