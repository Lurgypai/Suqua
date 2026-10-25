#include "StatsComponent.h"

#include "TopDownMoverComponent.h"

StatsComponent::StatsComponent(EntityId id_,
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
	int manaRegenPts,
	int manaRegenDelay,
	int manaRegenDelayPts,
	int moveSpeed,
	int moveSpeedPts,
	int carryWeight,
	int carryWeightPts
	) :
    id{ id_ },
	stats{
		StatVal{healthMax, healthMaxPts, 0, 0},
		StatVal{healthRegen, healthRegenPts, 0, 0},
		StatVal{healthRegenDelay, healthRegenDelayPts, 0, 0},
		StatVal{armorMax, armorMaxPts, 0, 0},
		StatVal{armorRegen, armorRegenPts, 0, 0},
		StatVal{armorRegenDelay, armorRegenDelayPts, 0, 0},
		StatVal{manaMax, manaMaxPts, 0, 0},
		StatVal{manaRegen, manaRegenPts, 0, 0},
		StatVal{manaRegenDelay, manaRegenDelayPts, 0, 0},
		StatVal{moveSpeed, moveSpeedPts, 0, 0},
		StatVal{carryWeight, carryWeightPts, 0, 0}
	}
{
}

int StatsComponent::get(Stat stat) {
	const auto& val = stats[stat];
	return ((static_cast<float>(val.base)
		* (static_cast<float>(val.pts) / 10.f))
		+ val.additiveBuff)
		* val.percentageBuff;
}

void StatsComponent::set(Stat stat, int base) {
	stats[stat].base = base;
}

void StatsComponent::setPts(Stat stat, int pts) {
	stats[stat].pts = pts;
}

void StatsComponent::modAdditiveBuff(Stat stat, int buff) {
	stats[stat].additiveBuff += buff;
}

void StatsComponent::modPercentageBuff(Stat stat, float buff) {
	stats[stat].percentageBuff += buff;
}

void StatsComponent::update(double timeDelta) {
	// health
	int curHealthRegen = get(Stat::health_regen);
	if (curHealthRegen > 0) {
		if (timeSinceHealthRegen < get(Stat::health_regen_delay)) {
			timeSinceHealthRegen += timeDelta * 1000;
		}
		else {
			health += curHealthRegen;
		}
	}
	int curMaxHealth = get(Stat::health_max);
	// health minimum
	if (curMaxHealth < 1) curMaxHealth = 1;
	if (health > curMaxHealth) health = curMaxHealth;

	// armor
	int curArmorRegen = get(Stat::armor_regen);
	if (timeSinceArmorRegen < get(Stat::armor_regen_delay)) {
		timeSinceArmorRegen += timeDelta * 1000;
	} else {
		armor += curArmorRegen;
	}
	int curMaxArmor = get(Stat::armor_max);
	if (armor > curMaxArmor) armor = curMaxArmor;

	// mana
	int curManaRegen = get(Stat::mana_regen);
	if (timeSinceManaRegen < get(Stat::mana_regen_delay)) {
		timeSinceManaRegen += timeDelta * 1000;
	} else {
		mana += curManaRegen;
	}
	int curMaxMana = get(Stat::mana_max);
	if (mana > curMaxMana) mana = curMaxMana;

	float curWeightPercent = static_cast<float>(weight) / get(Stat::carry_weight);
	float moveSpeedBuff = 0.f;
	if (curWeightPercent > 30.f && curWeightPercent <= 70.f) moveSpeedBuff = -20.f;
	else if (curWeightPercent > 70.f) moveSpeedBuff = -80.f;

	modPercentageBuff(Stat::move_speed, moveSpeedBuff);

	auto* mover = EntitySystem::GetComp<TopDownMoverComponent>(id);
	mover->setMoveSpeed(static_cast<float>(get(Stat::move_speed)) / 1000.f);
}
