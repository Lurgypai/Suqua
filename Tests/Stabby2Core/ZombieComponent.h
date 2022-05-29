#pragma once
#include "EntitySystem.h"
#include "Game.h"

#include <unordered_set>

namespace ZombieData {
	constexpr unsigned int ACTION_FRAME = 30;
	constexpr unsigned int DIR = 31;
	constexpr unsigned int STATE = 32;
	constexpr unsigned int HEALTH = 33;
	constexpr unsigned int FREEZE_FRAME = 34;
	constexpr unsigned int FREEZE_STATE = 35;
}

class ZombieComponent {
public:
	enum ZombieState : uint32_t {
		idle,
		moving,
		charging,
		attacking,
		hurt,
		freeze
	};

	ZombieComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update(Game& game);
	ZombieState getState() const;
	ZombieState getFrozenState() const;
	void damage(int damage);

	void markHit(EntityId target);
	bool hasHit(EntityId target);
	bool hitboxChanged();
	void clearMarked();

	int getDir() const;
	
	void beginFreeze();
private:
	void beginIdle();
	void idle_();

	void beginMoving();
	void moving_();

	void beginCharging();
	void charging_();

	void beginAttacking();
	void attacking_();

	void beginHurt();
	void hurt_();

	void freeze_();

	unsigned int idleTime;
	unsigned int chargeTime;
	float moveDecel;
	float attackDecel;
	float moveSpeed;
	Vec2f launchSpeed;

	std::unordered_set<EntityId> hitEntities;

	EntityId id;
};