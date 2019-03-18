#pragma once
#include "Vec2.h"
#include "NetworkTypes.h"
#include "PlayerLC.h"

#define PLAYER_WIDTH 4
#define PLAYER_HEIGHT 20

struct PlayerState {
	bool operator==(const PlayerState & other) {
		return when == other.when && pos == other.pos && vel == other.vel && state == other.state &&
			rollFrame == other.rollFrame && activeAttack == other.activeAttack && attackFrame == other.attackFrame &&
			health == other.health && stunFrames == other.stunFrames;
	}


	bool operator!=(const PlayerState & other) {
		return when != other.when || pos != other.pos || vel != other.vel || state != other.state ||
			rollFrame != other.rollFrame || activeAttack != other.activeAttack || attackFrame != other.attackFrame ||
			health != other.health || stunFrames != other.stunFrames;
	}

	State state;
	Time_t when;
	Vec2f pos;
	Vec2f vel;
	int rollFrame;
	unsigned int activeAttack;
	unsigned int attackFrame;
	int health;
	int stunFrames;
};