#pragma once
#include "ComponentMacros.h"
#include "Color.h"
#include <cstdint>

class OnHitComponent {
	CompMembers(OnHitComponent);

public:
	void update();
	Color c;
	int maxEffectTick;
private:
	std::uint32_t prevHealth;
	bool playingEffect;
	int tick;
};
