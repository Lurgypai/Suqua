#include "Attack.h"

void Attack::start() {
	currHitboxIndex = 0;
	elapsedTime = 0;
}

void Attack::update(int millis) {
	if (currHitboxIndex >= 0 && currHitboxIndex < hitboxes.size()) {
		const auto* hitbox = &hitboxes[currHitboxIndex];
		elapsedTime += millis * attackSpeed;
		while (elapsedTime > hitbox->duration) {
			elapsedTime -= hitbox->duration;
			++currHitboxIndex;
			//when we're out of hitboxes, stop
			if (currHitboxIndex == hitboxes.size())	return;
			hitbox = &hitboxes[currHitboxIndex];
		}
	}
}

void Attack::setPos(Vec2f pos_, int dir_) {
	pos = pos_;
	dir = dir_;
}

void Attack::setElapsedTime(uint32_t elapsedTime_)
{
	elapsedTime = elapsedTime_;
}

void Attack::addHitbox(std::vector<AABB> hitboxes_, int duration, int damage) {
	hitboxes.emplace_back(Hitbox{ hitboxes_, duration, damage });
}

const std::vector<Attack::Hitbox>& Attack::getHitboxes() const {
	return hitboxes;
}

const Attack::Hitbox* Attack::getCurrHitbox() const {
	if (currHitboxIndex >= 0 && currHitboxIndex < hitboxes.size()) return &hitboxes[currHitboxIndex];
	else return nullptr;
}

int Attack::getCurrHitboxIndex() const
{
	return currHitboxIndex;
}

void Attack::setCurrHitboxIndex(int index) {
	currHitboxIndex = index;
}

const Vec2f& Attack::getPos() const {
	return pos;
}

uint32_t Attack::getElapsedTime() const
{
	return elapsedTime;
}

int Attack::getDir() const {
	return dir;
}

bool Attack::finished() {
	return currHitboxIndex == hitboxes.size();
}
