#pragma once
#include "AABB.h"
#include <array>

//begin and end framess
struct Hitbox {
	AABB hit;
	Vec2f offset;
	unsigned int startup;
	unsigned int active;
	unsigned int ending;
};

class Attack {
public:
	Attack();
	void setActive(int i);
	void setFrame(int frame);
	void startAttacking();
	Hitbox* getActive();
	unsigned int getActiveId();
	unsigned int getCurrFrame();
	unsigned int getCurrentTotalFrames();
	void bufferNext();
	void update(Vec2f pos, Vec2f res, int facing);
private:
	std::array<Hitbox, 3> hitboxes;
	int active = 0;
	//current frame relative to the current attack
	unsigned int currFrame;
	bool nextIsBuffered;
	int restartDelay;
	int restartDelayMax;
};