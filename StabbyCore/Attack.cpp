#include "stdafx.h"
#include "Attack.h"
#include "PlayerData.h"

#include <iostream>

Attack::Attack() :
	hitboxes{
		Hitbox{AABB{Vec2f{0, 0}, Vec2f{14, 13}},
			Vec2f{2, -1}, 0, 20, 16},
		Hitbox{AABB{Vec2f{0, 0}, Vec2f{20, 20}},
			Vec2f{2, -1}, 0, 28, 16},
		Hitbox{AABB{Vec2f{0, 0}, Vec2f{30, 25}},
			Vec2f{-1, -5}, 8, 32, 8}
	},
	currFrame{0},
	nextIsBuffered{false},
	active{0},
	restartDelay{0},
	restartDelayMax{40}
{}

void Attack::setActive(int i) {
	if (i >= 0 && i < hitboxes.size())
		active = i;
}

void Attack::setFrame(int frame) {
	currFrame = frame;
}

void Attack::startAttacking() {
	if (restartDelay == restartDelayMax) {
		active = 1;
	}
}

Hitbox * Attack::getActive() {
	if (active != 0 && active <= hitboxes.size()) {
		Hitbox * hitbox = &hitboxes[active - 1];
		if (hitbox->startup < currFrame && currFrame < hitbox->startup + hitbox->active) {
			return hitbox;
		}
	}
	return nullptr;
}

unsigned int Attack::getActiveId(){
	return active;
}

unsigned int Attack::getCurrFrame() {
	return currFrame;
}

unsigned int Attack::getCurrentTotalFrames() {
	return hitboxes[active - 1].startup + hitboxes[active - 1].active + hitboxes[active - 1].ending;
}

void Attack::bufferNext() {
	nextIsBuffered = true;
}

void Attack::update(Vec2f pos, Vec2f res, int facing) {
	if (restartDelay != restartDelayMax) {
		++restartDelay;
		return;
	}
	//update pos
	if (active != 0) {
		Hitbox& hbox = hitboxes[active - 1];
		Vec2f offset = hbox.offset;
		if(facing == -1)
			offset.x = (-(hbox.hit.getRes().x - res.x)) - offset.x;

		hbox.hit.setPos(pos + offset);
	}

	//if a hitbox is running, keep moving through the frames. 
	if (active != 0 && currFrame != hitboxes[active - 1].startup + hitboxes[active - 1].active + hitboxes[active - 1].ending) {
		//skip delay. wait a frame so that we get multiple hits
		if (nextIsBuffered && currFrame > hitboxes[active - 1].startup + hitboxes[active - 1].active + 2 && active < 3) {
			++active;
			currFrame = 0;
			nextIsBuffered = false;
		}
		else {
			++currFrame;
		}
	}
	//otherwise reset (only if we weren't attacking)
	else if(active != 0) {
		active = 0;
		currFrame = 0;
		nextIsBuffered = false;
		restartDelay = 0;
	}
}
