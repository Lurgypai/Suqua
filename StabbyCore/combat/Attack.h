#pragma once
#include "AABB.h"
#include <array>

struct AttackStats {
	float damage;
	unsigned int stun;
	unsigned int staminaCost;

	float critChance;
	float critMultiplier;
	float vampirismChance;
	float vampirismMultiplier;
};

//begin and end framess
struct Hitbox {
	AABB hit;
	Vec2f offset;
	unsigned int startup;
	unsigned int active;
	unsigned int ending;

	AttackStats stats;
};

class Attack {
public:
	Attack();
	Attack(const std::string & id, int restartDelayMax_, double frameDelay_ = 1.0/120);
	void setActive(int i);
	void setFrame(int frame);
	void setSpeed(double newSpeed);
	bool canStartAttacking();
	bool getNextIsBuffered() const;
	void startAttacking();
	const Hitbox* getActive() const;
	const Hitbox* getHitbox(int pos) const;
	const size_t getHitboxCount() const;
	unsigned int getActiveId();
	unsigned int getCurrFrame();
	unsigned int getCurrentTotalFrames();
	AttackStats getStats() const;
	double getSpeed();
	void bufferNext();
	void update(double timeDelta, Vec2f pos, int facing);
	void addHitbox(Hitbox && hitbox);
	bool pollAttackChange();
	std::string getId();
private:
	std::vector<Hitbox> hitboxes;
	int active = 0;
	//current frame relative to the current attack
	unsigned int currFrame;
	bool nextIsBuffered;
	bool attackChanged;
	//delay for attacking again
	int restartDelay;
	int restartDelayMax;
	//delay between each frame of attacking
	double frameDelay;
	double elapsedTime;
	double speed;

	std::string id;
};