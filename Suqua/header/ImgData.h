#pragma once
#include "Color.h"

struct ImgData {
	Color colorOverlay;
	Vec2f objPos;
	Vec2i imgRes;
	Vec2i imgOffset;
	Vec2i objRes;
	Vec2f origin;
	Vec2f scale;
	float angle;
	float depth;
	float a;
	float pad0[1];
};