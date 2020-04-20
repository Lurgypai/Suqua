#include "AABB.h"

AABB::AABB(Vec2f pos_, Vec2f res_) :
	pos{pos_},
	res{res_}
{}

Vec2f AABB::center() const {
	return Vec2f{pos.x + res.x / 2, pos.y + res.y / 2};
}

void AABB::center(const Vec2f & newCenter) {
	//newpos = newcenter - res / 2;
	pos = Vec2f{ newCenter.x - res.x / 2, newCenter.y - res.y / 2 };
}

bool AABB::contains(Vec2f point) const {
	return (
		pos.x < point.x &&
		pos.y < point.y &&
		point.x < pos.x + res.x &&
		point.y < pos.y + res.y
		);
}

bool AABB::intersects(const AABB & other) const {
	return !(
		pos.x + res.x <= other.pos.x ||
		pos.y + res.y <= other.pos.y ||
		pos.x >= other.pos.x + other.res.x ||
		pos.y >= other.pos.y + other.res.y
		);
}

bool AABB::operator==(const AABB& other) const {
	return pos == other.pos && res == other.res;
}

bool AABB::operator!=(const AABB& other) const {
	return !(*this == other);
}