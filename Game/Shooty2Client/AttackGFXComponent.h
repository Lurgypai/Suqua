#pragma once
#include "ComponentMacros.h"

class AttackGFXComponent {
	CompMembers(AttackGFXComponent);

public:
	void update();
	int maxEffectTick;
private:
    bool prevButtonDown;
	bool playingEffect;
	int tick;
};
