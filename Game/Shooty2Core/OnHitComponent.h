#pragma once
#include "ComponentMacros.h"
#include "Color.h"
#include <cstdint>

class OnHitComponent {
	CompMembers(OnHitComponent);

public:
    OnHitComponent(EntityId id_,
            const Color& c_ = Color{1.f, 1.f, 1.f, 1.f}
            );

	void update();
	Color c;
	int maxEffectTick;
private:
	std::uint32_t prevHealth;
	bool playingEffect;
	int tick;
};
