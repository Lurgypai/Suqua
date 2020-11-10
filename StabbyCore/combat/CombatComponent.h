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
friend class AttackManager;

public:
	CombatComponent(EntityId id = 0);
	CombatComponent(const CombatComponent& other);
	virtual ~CombatComponent() {};
	EntityId getId() const;

	AABB getBoundingBox();
	const Hitbox * getActiveHitbox() const;
	unsigned int getStun();
	unsigned int getStaminaCost();
	bool isStunned();
	bool isAlive();
	bool isFrozen();
	bool isNear(const CombatComponent& other);

	void damage(unsigned int i);
	void heal(unsigned int i);
	void stun(unsigned int i);
	void freeze();
	bool startAttacking();
	bool bufferNextAttack();
	bool hasHitEntity(const EntityId & target);
	void useStamina(uint32_t amount);

	void setActiveHitbox(int i);
	void setFrame(int frame);
	void setAttackSpeed(double newSpeed);

	void setAttack(const std::string& attackId);
	const Attack& getAttack() const;
	unsigned int getRemainingFrames() const;

	//runs damage calculations and returns the result
	int rollDamage();
	EntityId getLastAttacker();

	CombatStats stats;
	int health;
	uint32_t stamina;
	uint32_t staminaMax;
	uint32_t staminaRechargeFrame;
	uint32_t freezeFrame;
	unsigned int teamId;
	unsigned int stunFrame;
	bool invulnerable;
	bool ignoreDamage;
	std::vector<Hurtbox> hurtboxes;
private:
	void updateStun();
	void updateHurtboxes();
	void updateStamina();
	void updateFreezeFrame();
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

	uint32_t staminaRechargeMax;
	uint32_t freezeFrameMax;
	Attack* attack;
	std::unordered_map<std::string, Attack> attacks;

	static std::unordered_map<std::string, Attack> AttackMap;
};