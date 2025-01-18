#pragma once
#include "ComponentMacros.h"

class AttackGFXComponent {
	CompMembers(AttackGFXComponent);

public:
    AttackGFXComponent(EntityId id_, int maxEffectTick_);

	void update();
	int maxEffectTick;
private:
    bool prevButtonDown;
	bool playingEffect;
	int tick;
};
