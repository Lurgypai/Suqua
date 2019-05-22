#include "stdafx.h"
#include "PlayerLC.h"
#include "PhysicsAABB.h"
#include "PlayerData.h"

#include <iostream>

PlayerLC::PlayerLC(EntityId id_) :
	id{ id_ },
	pos{ 0, 0 },
	vel{ 0, 0 },
	maxXVel{ 50 },
	xAccel{ 10 },
	gravity{ 3 },
	jumpSpeed{ 120 },
	canJump{ false },
	attack{},
	facing{1},
	prevButton2{false},
	prevButton3{ false },
	isBeingHit{false},
	health{3},
	rollVel{340},
	storedVel{0},
	rollFrame{0},
	rollFrameMax{30},
	stunFrame{0},
	stunFrameMax{80},
	stunSlideSpeed{10},
	state{State::free},
	deathFrame{0},
	deathFrameMax{200},
	collider{ pos, Vec2f{static_cast<float>(PLAYER_WIDTH), static_cast<float>(PLAYER_HEIGHT)} }
{}

void PlayerLC::update(double timeDelta, const Controller & controller, const Stage& stage) {

	bool attackToggledDown{ false };
	bool currButton2 = controller[ControllerBits::BUTTON_2];
	if (prevButton2 != currButton2) {
		prevButton2 = currButton2;
		if (currButton2) {
			attackToggledDown = true;
		}
	}

	//always update the attack. Hitboxes should only be out when we are stuck in attack mode.
	attack.update(pos, collider.getRes(), facing);

	switch (state) {
	case State::stunned:
		if (stunFrame == 0) {
			vel.x = 0;
			++stunFrame;
		}
		else if (stunFrame != stunFrameMax) {
			++stunFrame;
		}
		else {
			stunFrame = 0;
			state = State::free;
		}
		break;
	case State::attacking:
		vel.x = 0;
		if (attack.getActiveId() == 0)
			state = State::free;
		else if(attackToggledDown)
			attack.bufferNext();

		break;
	case State::rolling:
		if (rollFrame != rollFrameMax) {
			rollFrame++;
		}
		else {
			rollFrame = 0;
			vel.x = storedVel;
			state = State::free;
		}
		break;
	case State::free:
		free(controller, attackToggledDown);
		break;
	case State::dead:
		++deathFrame;

		if (deathFrame == deathFrameMax) {
			deathFrame = 0;
			respawn();
		}
		break;
	}

	vel.y += gravity;

	collider.setPos(pos);
	collider.setVel(vel);



	pos = collider.handleCollision(stage.getCollider(), timeDelta);

	if (collider.getVel().y == 0 && vel.y > 0) {
		canJump = true;
	}
	else {
		canJump = false;
	}

	vel = collider.getVel();
	if (pos.y > 1000) {
		respawn();
	}
}

void PlayerLC::setPos(Vec2f pos_) {
	pos = pos_;
}

Vec2f PlayerLC::getPos() const {
	return pos;
}

Vec2f PlayerLC::getVel() const {
	return vel;
}

Vec2f PlayerLC::getRes() const {
	return collider.getRes();
}

EntityId PlayerLC::getId() const {
	return id;
}

Attack & PlayerLC::getAttack() {
	return attack;
}

State PlayerLC::getState() {
	return state;
}

int PlayerLC::getActiveId() {
	return attack.getActiveId();
}

int PlayerLC::getRollFrame() {
	return rollFrame;
}

int PlayerLC::getStunFrame() {
	return stunFrame;
}

int PlayerLC::getFacing() {
	return facing;
}

int PlayerLC::getHealth() {
	return health;
}

void PlayerLC::damage(int amount) {
	health -= amount;
	state = State::stunned;

	if (health <= 0)
		die();
}

void PlayerLC::die() {
	state = State::dead;
	rollFrame = 0;
	stunFrame = 0;
	attack.setActive(0);
	attack.setFrame(0);
	vel = { 0, 0 };
}

void PlayerLC::kill() {
	health = 0;
	respawn();
}

void PlayerLC::respawn() {
	state = State::free;
	health = 3;
	pos = {static_cast<float>( -PLAYER_WIDTH / 2), static_cast<float>(-PLAYER_HEIGHT) };
	vel = { 0, 0 };
	attack.setActive(0);
	attack.setFrame(0);
}

void PlayerLC::free(const Controller & controller, bool attackToggledDown_) {

	if (attackToggledDown_) {
		state = State::attacking;
		if (attack.getActiveId() == 0) {
			attack.startAttacking();
		}
	}

	//try to start attack
	if (state != State::attacking) {
		bool currButton3 = controller[ControllerBits::BUTTON_3];
		if (prevButton3 != currButton3) {
			prevButton3 = currButton3;
			if (currButton3) {
				state = State::rolling;
				storedVel = vel.x;
				vel.x = facing * rollVel;
			}
		}
	}

	if (state == State::free) {
		int dir{ 0 };
		if (controller[ControllerBits::LEFT]) {
			--dir;
		}
		if (controller[ControllerBits::RIGHT]) {
			++dir;
		}
		if (controller[ControllerBits::UP]) {
			if (canJump) {
				vel.y = -jumpSpeed;
				canJump = false;
			}
		}
		//otherwise do
		if (dir == 0) {
			if (vel.x > 0)
				vel.x -= xAccel;
			else if (vel.x < 0)
				vel.x += xAccel;
		}
		else {
			facing = dir;
			vel.x += (dir * xAccel);
		}

		if (vel.x > maxXVel)
			vel.x = maxXVel;
		else if (vel.x < -maxXVel)
			vel.x = -maxXVel;
	}
}
