#pragma once
#include "Vec2.h"

#include <vector>

template <typename T>
class AABB_ {
public:
	enum class Direction {
		up,
		down,
		left,
		right
	};

	AABB_(Vec2<T> pos_ = { 0, 0 }, Vec2<T> res_ = {0, 0});
	Vec2<T> center() const;

	std::vector<Vec2<T>> toPoints() const;

	void center(const Vec2<T> & newCenter);

	bool contains(Vec2<T> point) const;
	bool intersects(const AABB_& other) const;

	bool operator==(const AABB_& other) const;
	bool operator!=(const AABB_& other) const;

	Vec2<T> pos;
	Vec2<T> res;
};

template<typename T>
AABB_<T>::AABB_(Vec2<T> pos_, Vec2<T> res_) :
	pos{pos_},
	res{res_}
{}

template<typename T>
Vec2<T> AABB_<T>::center() const {
	return Vec2<T>{pos.x + res.x / 2, pos.y + res.y / 2};
}

template<typename T>
void AABB_<T>::center(const Vec2<T> & newCenter) {
	//newpos = newcenter - res / 2;
	pos = Vec2<T>{ newCenter.x - res.x / 2, newCenter.y - res.y / 2 };
}

template<typename T>
bool AABB_<T>::contains(Vec2<T> point) const {
	return (
		pos.x <= point.x &&
		pos.y <= point.y &&
		point.x <= pos.x + res.x &&
		point.y <= pos.y + res.y
		);
}

template<typename T>
bool AABB_<T>::intersects(const AABB_<T> & other) const {
	return !(
		pos.x + res.x <= other.pos.x ||
		pos.y + res.y <= other.pos.y ||
		pos.x >= other.pos.x + other.res.x ||
		pos.y >= other.pos.y + other.res.y
		);
}

template<typename T>
bool AABB_<T>::operator==(const AABB_<T>& other) const {
	return pos == other.pos && res == other.res;
}

template<typename T>
bool AABB_<T>::operator!=(const AABB_<T>& other) const {
	return !(*this == other);
}

using AABB = AABB_<float>;
using AABBi = AABB_<int32_t>;
