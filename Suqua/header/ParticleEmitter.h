#pragma once
#include <string>
#include "Vec2.h"

class ParticleEmitter {
public:
	Vec2f pos;
	float radius;
	std::string particleTag;
	float emitDelay;
	bool on;

	ParticleEmitter() = default;
	ParticleEmitter(Vec2f pos_, float radius_, std::string particleTag_, float emitDelay_);

	void update(float timeDelta);
private:
	float elapsedTime;
};
