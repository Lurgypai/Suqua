#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "Attack.h"
#include "Game.h"

#include <fstream>
#include <unordered_set>

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
	constexpr unsigned int ATTACK_ELAPSED_TIME = 41;
	constexpr unsigned int PERCENT = 42;
	constexpr unsigned int STUN_FRAME = 43;
	constexpr unsigned int PREV_HITBOX = 44;
	constexpr unsigned int PREV_ATTACK = 45;
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
		grounded_attack,
		respawning,
		hitstun
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
	void doRespawn();
	void damage(int damage);
	void launch(Vec2f launchDir);

	void markHit(EntityId otherPlayer);
	bool hasHit(EntityId otherPlayer);
	bool hitboxChanged();
	void clearMarked();

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

	void beginRespawn();
	void _respawning(const Game& game);

	void beginHitstun();
	void _hitstun();

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
	std::unordered_set<EntityId> hitEntities;

	static std::ofstream playerLog;
};