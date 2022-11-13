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
	constexpr unsigned int BUFFER_ATTACK = 37;
	constexpr unsigned int STATE = 38;
	constexpr unsigned int CURR_ATTACK = 39;
	constexpr unsigned int CURR_HITBOX = 40;
	constexpr unsigned int ATTACK_ELAPSED_TIME = 41;
	constexpr unsigned int PERCENT = 42;
	constexpr unsigned int PREV_HITBOX = 44;
	constexpr unsigned int PREV_ATTACK = 45;
	constexpr unsigned int FREEZE_STATE = 46;
	constexpr unsigned int FREEZE_FRAME = 47;
}

class PlayerComponent {
public:
	enum PlayerState : uint32_t {
		idle,
		walking,
		dodge,
		grounded_attack,
		respawning,
		hitstun,
		freeze
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
	PlayerState getFrozenState() const;
	void doRespawn();
	void damage(int damage);

	void markHit(EntityId otherPlayer);
	bool hasHit(EntityId otherPlayer);
	bool hitboxChanged();
	void clearMarked();

	std::optional<Attack> getCurrAttack() const;
	PlayerAttack getCurrAttackId() const;


	void beginFreeze();
private:
	void _idle();
	void _walking();

	inline void beginDodge();
	void _dodge();

	void beginGroundedAttack();
	void _groundedAttack(const Game& game);

	void beginRespawn();
	void _respawning(const Game& game);

	void beginHitstun();
	void _hitstun();

	void _freeze();

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
	float dodgeSpeed;
	float jumpVel;
	float shortHopVel;
	float moveSpeed;
	float groundedDecel;

	EntityId id;

	std::vector<Attack> attacks;
	std::unordered_set<EntityId> hitEntities;

	static std::ofstream playerLog;
};