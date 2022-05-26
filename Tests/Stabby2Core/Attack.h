#pragma once
#include "AABB.h"
#include <vector>
#include <optional>

class Attack {
public:
	struct Hitbox {
		std::vector<AABB> hitboxes;
		int duration;
		int launchSpeed;
		float launchAngle;
	};

	Attack() = default;

	void start();
	void update(int millis);
	void setPos(Vec2f pos_, int dir_);
	void setElapsedTime(uint32_t elapsedTime_);
	//specialize this for just an AABB
	template<typename T>
	bool intersects(const T& other) const;

	void addHitbox(std::vector<AABB> hitboxes_, int duration, int launchSpeed, float launchAngle);
	const std::vector<Hitbox>& getHitboxes() const;
	const Hitbox* getCurrHitbox() const;
	int getCurrHitboxIndex() const;
	void setCurrHitboxIndex(int index);
	const Vec2f& getPos() const;
	uint32_t getElapsedTime() const;
	int getDir() const;

	bool finished();

	float attackSpeed = 1.0f;
private:
	int currHitboxIndex;
	unsigned int elapsedTime;
	int dir;
	Vec2f pos;
	std::vector<Hitbox> hitboxes;
};

template<typename T>
inline bool Attack::intersects(const T& other) const {
	if (currHitboxIndex < 0 || currHitboxIndex >= hitboxes.size()) return false;
	for (const AABB& collider : other) {
		const Hitbox& currHitbox = hitboxes[currHitboxIndex];
		for (const AABB& hitbox : currHitbox.hitboxes) {
			AABB projectedHitbox = {
				{ pos.x + (hitbox.pos.x * dir) - (dir < 0 ? hitbox.res.x : 0),
				pos.y + hitbox.pos.y },
				{ hitbox.res.x, hitbox.res.y}
			};

			if (projectedHitbox.intersects(collider)) return true;
		}
	}
	return false;
}

template<>
inline bool Attack::intersects<AABB>(const AABB& other) const {
	if (currHitboxIndex < 0 || currHitboxIndex >= hitboxes.size()) return false;
	const Hitbox& currHitbox = hitboxes[currHitboxIndex];
	for (const AABB& hitbox : currHitbox.hitboxes) {
		AABB projectedHitbox = {
			{ pos.x + (hitbox.pos.x * dir) - (dir < 0 ? hitbox.res.x : 0),
			pos.y + hitbox.pos.y },
			{ hitbox.res.x, hitbox.res.y}
		};

		if (projectedHitbox.intersects(other)) return true;
	}
	return false;
}