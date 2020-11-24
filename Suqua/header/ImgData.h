#pragma once

struct ImgData {
	Vec2f objPos;
	Vec2i imgRes;
	Vec2i imgOffset;
	Vec2i objRes;
	Vec2f origin;
	Vec2f scale;
	float angle;
	float depth;
	float pad[2];
};