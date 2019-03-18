#pragma once
#include "AABBLC.h"
#include <array>

//begin and end framess
struct Hitbox {
	AABBLC hit;
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
	Hitbox* getActive();
	unsigned int getActiveId();
	unsigned int getCurrFrame();
	unsigned int getCurrentTotalFrames();
	void bufferNext();
	void forward(Vec2f pos, int facing);
private:
	std::array<Hitbox, 3> hitboxes;
	int active = 0;
	//current frame relative to the current attack
	unsigned int currFrame;
	bool nextIsBuffered;
};