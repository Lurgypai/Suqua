#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "Controller.h"
#include "Attack.h"
enum class State {
	free,
	attacking,
	rolling,
	stunned,
	dead
};

class PlayerLC {
public:
	PlayerLC(EntityId id_ = 0);
	void update(double timeDelta, const Controller& controller);
	void setPos(Vec2f pos);
	Vec2f getPos() const;
	Vec2f getVel() const;
	EntityId getId() const;
	Attack& getAttack();
	State getState();
	int getActiveId();
	int getRollFrame();
	int getStunFrame();
	int getFacing();
	int getHealth();
	void damage(int amount);
	void die();
	void kill();
	void respawn();
protected:
	void free(const Controller & controller, bool attackToggledDown_);

	Vec2f pos;
	Vec2f vel;
	//as a multiple of acceleration
	int maxXVel;
	float xAccel;
	float gravity;
	float jumpSpeed;
	bool canJump;
	//for only getting hit once per slash
	bool isBeingHit;

	State state;

	Attack attack;
	bool attackBuffered;

	EntityId id;
	int facing;
	bool prevButton2;
	bool prevButton3;
	int health;

	float rollVel;
	float storedVel;
	int rollFrame;
	int rollFrameMax;

	float stunSlideSpeed;
	int stunFrame;
	int stunFrameMax;

	int deathFrame;
	int deathFrameMax;
};