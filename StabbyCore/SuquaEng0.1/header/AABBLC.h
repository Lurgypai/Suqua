#pragma once
#include "Vec2.h"
#include <unordered_map>
#include "EntitySystem.h"

//create an AABBLC class that holds velocity, and handles moving bounding boxes, including collision.

class AABBLC {
public:
	enum class Direction {
		up,
		down,
		left,
		right
	};

	AABBLC(EntityId id_ = 0);
	AABBLC(Vec2f pos_, Vec2f res_);
	Vec2f getPos() const;
	Vec2f getRes() const;
	Vec2f center() const;

	void center(const Vec2f & newCenter);
	void setPos(const Vec2f& other);
	void setRes(const Vec2f& res_);

	bool contains(Vec2f point);
	bool intersects(AABBLC& other);
	EntityId getId() const;

protected:
	EntityId id;
	Vec2f pos;
	Vec2f res;
};
