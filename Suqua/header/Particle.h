#pragma once
#include "Vec2.h"
#include "Color.h"

struct Particle {
	Color color;
	Vec2f pos;
	float angle;
	float vel;
	int life;
	int death;
	float pad[2];
};