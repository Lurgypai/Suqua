#include "stdafx.h"
#include "PhysicsAABBLC.h"

PhysicsAABBLC::PhysicsAABBLC(EntityId id_) : AABBLC{id_}
{}

Vec2f PhysicsAABBLC::collides(AABBLC & other, double timeDelta) {

	Vec2f oldPos = pos;
	pos = { pos.x + vel.x * static_cast<float>(timeDelta), pos.y + vel.y * static_cast<float>(timeDelta) };

	Vec2f overlap{ 0, 0 };

	if (intersects(other)) {
		//moving down
		if (vel.y > 0) {
			//if we were above
			if (oldPos.y + res.y <= other.getPos().y) {
				overlap.y = (pos.y + res.y) - other.getPos().y;
			}
		}
		//moving up
		else if (vel.y < 0) {
			//below
			if (oldPos.y >= other.getPos().y + other.getRes().y) {
				overlap.y = pos.y - (other.getPos().y + other.getRes().y);
			}
		}

		//moving right
		if (vel.x > 0) {
			//if we were to the left of it
			if (oldPos.x + res.x <= other.getPos().x) {
				overlap.x = (pos.x + res.x) - other.getPos().x;
			}
		}
		//moving left
		else if (vel.x < 0) {
			if (oldPos.x >= other.getPos().x + other.getRes().x) {
				overlap.x = pos.x - (other.getPos().x + other.getRes().x);
			}
		}
	}
	pos = oldPos;
	return overlap;
}

float PhysicsAABBLC::collides(AABBLC & other, double timeDelta, Direction dir) {

	Vec2f oldPos = pos;
	pos = { pos.x + vel.x * static_cast<float>(timeDelta), pos.y + vel.y * static_cast<float>(timeDelta) };

	float overlap{ 0.0f };

	switch (dir) {
	case Direction::down:
		if (vel.y > 0) {
			//if we were above
			if (oldPos.y + res.y <= other.getPos().y) {
				overlap = (pos.y + res.y) - other.getPos().y;
			}
		}
		break;
	case Direction::up:
		if (vel.y < 0) {
			//below
			if (oldPos.y >= other.getPos().y + other.getRes().y) {
				overlap = pos.y - (other.getPos().y + other.getRes().y);
			}
		}
		break;
	case Direction::right:
		if (vel.x > 0) {
			//if we were to the left of it
			if (oldPos.x + res.x <= other.getPos().x) {
				overlap = (pos.x + res.x) - other.getPos().x;
			}
		}
		break;
	case Direction::left:
		//moving left
		if (vel.x < 0) {
			if (oldPos.x >= other.getPos().x + other.getRes().x) {
				overlap = pos.x - (other.getPos().x + other.getRes().x);
			}
		}
		break;
	}
	pos = oldPos;
	return overlap;
}

Vec2f PhysicsAABBLC::getVel() const {
	return vel;
}

void PhysicsAABBLC::setVel(const Vec2f & vel_) {
	vel = vel_;
}