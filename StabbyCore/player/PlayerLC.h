#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "Controller.h"
#include "../stage/Stage.h"
#include "PhysicsComponent.h"
#include "PlayerStateComponent.h"

#include "../combat/Attack.h"
#include "../combat/CombatComponent.h"

class PlayerLC {
public:
	PlayerLC(EntityId id_ = 0);
	void update(double timeDelta);
	PhysicsComponent * getPhysics();
	Vec2f getVel() const;
	Vec2f getRes() const;
	void respawn(const Vec2f & spawnPos);
	bool shouldRespawn();
	void shouldRespawn(bool newShouldRespawn);
	void kill();
	//kill and make them select a spawn point immediately
	void chooseSpawn();

	EntityId getId() const;

	void setState(const PlayerState& state);
	PlayerState getState();

	const static int PLAYER_WIDTH = 4;
	const static int PLAYER_HEIGHT = 20;
protected:
	EntityId id;

	void free(const Controller & controller, bool attackToggledDown_);
	//as a multiple of acceleration
	float jumpSpeed;
	//for only getting hit once per slash
	bool isBeingHit;
	double horizontalAccel;
	double stepDistance;
	double climbDistance;
	bool attackBuffered;

	bool prevButton1;
	bool prevButton2;
	bool prevButton3;

	float rollVel;
	float storedVel;
	int rollFrameMax;

	int healFrameMax;
	int healDelayMax;
	uint32_t rollCost;

	int deathFrameMax;

	std::vector<AABB> hurtboxes;
	
	State storedState;
};

/*
set everything with direction to use direction component
complete the implementation of the CombatComponent
add the AttackStats struct

Begin implementation of Effects
*/