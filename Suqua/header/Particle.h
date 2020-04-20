#pragma once
#include "Vec2.h"

struct Particle {
	Vec2f pos;
	float angle;
	float vel;
	int life;
	int death; 
	float pad[2];
};