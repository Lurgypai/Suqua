#pragma once

#include <array>

#include "ComponentMacros.h"

class StatsComponent {
    CompMembers(StatsComponent);
public:
	StatsComponent(EntityId id_,
		int healthMax,
		int healthMaxPts,
		int healthRegen,
		int healthRegenPts,
		int healthRegenDelay,
		int healthRegenDelayPts,
		int armorMax,
		int armorMaxPts,
		int armorRegen,
		int armorRegenPts,
		int armorRegenDelay,
		int armorRegenDelayPts,
		int manaMax,
		int manaMaxPts,
		int manaRegen,
		int manaRegenMaxPts,
		int manaRegenDelay,
		int manaRegenDelayPts,
		int moveSpeed,
		int moveSpeedPts,
		int carryWeight,
		int carryWeightPts
	);

	// delays are in ms
	enum Stat : size_t {
		health_max,
		health_regen,
		health_regen_delay,

		armor_max,
		armor_regen,
		armor_regen_delay,

		mana_max,
		mana_regen,
		mana_regen_delay,

		move_speed,

		carry_weight
	};

	int get(Stat stat);
	void set(Stat stat, int base);
	void setPts(Stat stat, int pts);
	void modAdditiveBuff(Stat stat, int buff);
	void modPercentageBuff(Stat stat, float buff);

	void update(double timeDelta);
	void clearBuffs();

	// public only for network data component, do not touch!!
	// maybe I should figure out a better design
	std::int32_t health;
	std::int32_t mana;
	std::int32_t armor;
	std::int32_t weight;
private:

	struct StatVal {
		int base;
		int pts;
		int additiveBuff;
		float percentageBuff;
	};

	std::array<StatVal, 11> stats;
	float timeSinceHealthRegen;
	float timeSinceArmorRegen;
	float timeSinceManaRegen;

	/*
	Stat healthMax;
	Stat healthRegen;
	Stat healthRegenDelay;

	Stat manaMax;
	Stat manaRegen;
	Stat manaRegenDelay;

	Stat moveSpeed;

	Stat carryWeight;
	*/
};

/*
stats
health
    current health
    max health
    health regen
    health regen delay
mana
    current mana
    max mana
    mana regen
    mana regen delay
shields
	current shields
	max shields
	shield regen rate
	shield regen delay
overheal
	current overheal
	overheal degen rate
move speed
	move speed
carry weight
	carry weight
*/
