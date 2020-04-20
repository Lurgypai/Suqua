#pragma once
#include "Vec2.h"
#include <unordered_map>
#include "EntitySystem.h"

//create an AABB class that holds velocity, and handles moving bounding boxes, including collision.

class AABB {
public:
	enum class Direction {
		up,
		down,
		left,
		right
	};

	AABB(Vec2f pos_ = { 0, 0 }, Vec2f res_ = {0, 0});
	Vec2f center() const;

	void center(const Vec2f & newCenter);

	bool contains(Vec2f point) const;
	bool intersects(const AABB& other) const;

	bool operator==(const AABB& other) const;
	bool operator!=(const AABB& other) const;

	Vec2f pos;
	Vec2f res;
};
