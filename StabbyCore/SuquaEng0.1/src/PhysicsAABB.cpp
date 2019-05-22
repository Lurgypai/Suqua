#include "stdafx.h"
#include "PhysicsAABB.h"

PhysicsAABB::PhysicsAABB(Vec2f pos_, Vec2f res_) : AABB{pos_, res_}
{}

Vec2f PhysicsAABB::collides(const AABB & other, double timeDelta) {

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

float PhysicsAABB::collides(const AABB & other, double timeDelta, Direction dir) {

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



Vec2f PhysicsAABB::handleCollision(const AABB& other, double timeDelta) {		//place we are updating from
	Vec2f currPos = pos;
	//plce we are updating too
	Vec2f newPos = { currPos.x + vel.x * static_cast<float>(timeDelta), currPos.y + vel.y * static_cast<float>(timeDelta) };

	AABB projection{ newPos, res };

	//handle collisions
	if (other.intersects(projection)) {
		Vec2f overlap = collides(other, timeDelta);
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

Vec2f PhysicsAABB::getVel() const {
	return vel;
}

void PhysicsAABB::setVel(const Vec2f & vel_) {
	vel = vel_;
}