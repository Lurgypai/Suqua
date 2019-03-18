#include "stdafx.h"
#include "AABBLC.h"

AABBLC::AABBLC(EntityId id_) : id{id_} {
}

AABBLC::AABBLC(Vec2f pos_, Vec2f res_) :
	pos{pos_},
	res{res_}
{}


Vec2f AABBLC::getPos() const {
	return pos;
}

Vec2f AABBLC::getRes() const {
	return res;
}

Vec2f AABBLC::center() const {
	return Vec2f{pos.x + res.x / 2, pos.y + res.y / 2};
}

void AABBLC::center(const Vec2f & newCenter) {
	//newpos = newcenter - res / 2;
	pos = Vec2f{ newCenter.x + res.x / 2, newCenter.y + res.y / 2 };
}

void AABBLC::setPos(const Vec2f & other) {
	pos = other;
}

void AABBLC::setRes(const Vec2f & res_) {
	res = res_;
}

bool AABBLC::contains(Vec2f point) {
	return (
		pos.x < point.x &&
		pos.y < point.y &&
		point.x < pos.x + res.x &&
		point.y < pos.y + res.y
		);
}

bool AABBLC::intersects(AABBLC & other) {
	return !(
		pos.x + res.x <= other.pos.x ||
		pos.y + res.y <= other.pos.y ||
		pos.x >= other.pos.x + other.res.x ||
		pos.y >= other.pos.y + other.res.y
		);
}

EntityId AABBLC::getId() const {
	return id;
}
