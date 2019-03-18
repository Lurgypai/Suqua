#pragma once
#include "stdafx.h"
#include "AABBLC.h"

class PhysicsAABBLC : public AABBLC {
public:
	PhysicsAABBLC(EntityId id_ = 0);
	Vec2f collides(AABBLC & other, double timeDelta);
	//Dir: 0 - Up, 1 - Down, 2 - Left, Else - Right
	float collides(AABBLC & other, double timeDelta, Direction dir);

	template<typename Container>
	Vec2f handleCollisions(Container c, double timeDelta);
	Vec2f getVel() const;
	void setVel(const Vec2f & vel_);

	Vec2f vel;
};

template<typename Container>
inline Vec2f PhysicsAABBLC::handleCollisions(Container c, double timeDelta) {		//place we are updating from
	Vec2f currPos = pos;
	//plce we are updating too
	Vec2f newPos = { currPos.x + vel.x * static_cast<float>(timeDelta), currPos.y + vel.y * static_cast<float>(timeDelta) };

	AABBLC projection{ newPos, res };

	//grab new intersections
	std::vector<AABBLC*> intersectors;
	for (auto& r : c) {
		if (!r.isFree) {
			AABBLC* other_collider = &r.val;
			if (other_collider->intersects(projection))
				intersectors.push_back(other_collider);
		}
	}

	//handle collisions
	for (auto iter = intersectors.begin(); iter != intersectors.end(); ++iter) {
		Vec2f overlap = collides(**iter, timeDelta);
		//horizontal collision
		if (overlap.x != 0.0f && overlap.y == 0.0f) {
			vel.x = 0.0f;
		}
		//vertical collision
		else if (overlap.x == 0.0f && overlap.y != 0.0f) {
			vel.y = 0.0f;
		}
		//corner collision
		else if (overlap.x != 0.0f && overlap.y != 0.0f) {
			float absXVel = abs(vel.x);
			float absYVel = abs(vel.y);
			if (absXVel > absYVel) {
				//this means don't resolve collisions allong the x axis
				overlap.x = 0;
				//and stop moving allong the y axis
				vel.y = 0;
			}
			else {
				overlap.y = 0;
				vel.x = 0;
			}
		}
		newPos -= overlap;
	}
	pos = newPos;
	return newPos;
}