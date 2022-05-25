#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "Attack.h"
#include "Game.h"

#include <fstream>

namespace PlayerData {
	constexpr unsigned int ACTION_FRAME = 30;
	constexpr unsigned int DIR = 31;
	constexpr unsigned int DODGE_DIR = 32;
	constexpr unsigned int AIRDODGE_X_DIR = 33;
	constexpr unsigned int AIRDODGE_Y_DIR = 34;
	constexpr unsigned int FULLHOP = 35;
	constexpr unsigned int AIRDODGED = 36;
	constexpr unsigned int BUFFER_ATTACK = 37;
	constexpr unsigned int STATE = 38;
	constexpr unsigned int CURR_ATTACK = 39;
	constexpr unsigned int CURR_HITBOX = 40;
}

class PlayerComponent {
public:
	enum PlayerState : uint32_t {
		idle,
		walking,
		jumpsquat,
		airborn,
		landing,
		dodge,
		airdodge,
		grounded_attack
	};

	enum PlayerAttack : uint32_t {
		slash1,
		slash2,
		slash3,
		dair,
		none
	};

	PlayerComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update(const Game& game);
	PlayerState getState() const;

	std::optional<Attack> getCurrAttack() const;
	PlayerAttack getCurrAttackId() const;

private:
	void _idle();
	void _walking();

	inline void beginJumpsquat();
	void _jumpsquat();

	void _airborn();
	void _landing();

	inline void beginDodge();
	void _dodge();

	void beginAirdodge();
	void _airdodge();

	void beginGroundedAttack();
	void _groundedAttack(const Game& game);

	void decelerate();

	//int actionFrame;
	//int dir;
	//int dodgeDir;
	//Vec2f airdodgeDir;
	//bool fullHop;
	//bool airDodged;
	//bool bufferAttack;
	//PlayerState state;
	//PlayerAttack currAttack;

	int jumpSquatMax;
	int landingMax;
	int dodgeMax;
	int airdodgeMax;
	float dodgeSpeed;
	float jumpVel;
	float shortHopVel;
	float moveSpeed;
	float groundedDecel;
	float airdodgeSpeed;


	EntityId id;

	std::vector<Attack> attacks;

	static std::ofstream playerLog;
};