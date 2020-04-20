#pragma once
#include "EntitySystem.h"
#include "CombatSystem.h"
#include "AABB.h"
#include "Attack.h"
#include "DynamicBitset.h"

struct Hurtbox {
	//offset from the middle bottom of the entity being described
	Vec2f offset;
	AABB box;
};

struct CombatStats {
	int maxHealth;
	int regeneration;
	int shields;

	float baseDamage;
	float critChance;
	float critMultiplier;
	float vampirismChance;
	float vampirismMultiplier;
};

class CombatComponent {

friend class CombatSystem;

public:
	CombatComponent(EntityId id = 0);
	virtual ~CombatComponent() {};
	EntityId getId() const;

	AABB getBoundingBox();
	const Hitbox * getActiveHitbox() const;
	unsigned int getStun();
	unsigned int getStaminaCost();
	bool isStunned();
	bool isAlive();

	void damage(unsigned int i);
	void heal(unsigned int i);
	void stun(unsigned int i);
	bool startAttacking();
	bool bufferNextAttack();
	bool hasHitEntity(const EntityId & target);
	void useStamina(uint32_t amount);

	//runs damage calculations and returns the result
	int rollDamage();
	EntityId getLastAttacker();

	Attack attack;
	CombatStats stats;
	int health;
	uint32_t stamina;
	uint32_t staminaRechargeFrame;
	unsigned int teamId;
	unsigned int stunFrame;
	bool invulnerable;
	std::vector<Hurtbox> hurtboxes;
private:
	void updateStun();
	void updateHurtboxes();
	void updateStamina();
	void addHitEntity(EntityId hit);
	void clearHitEntities();

	//effects triggers
	void onAttackLand();
	void onKill();
	void onDeath();
	void onHeal(unsigned int amount);
	void onDamage(unsigned int amount);
	void onStun(unsigned int amount);

	EntityId id;
	EntityId lastAttacker;
	//Index is the EntityId, bool is wether they've been hit
	DynamicBitset hitEntities;

	uint32_t staminaMax;
	uint32_t staminaRechargeMax;
};