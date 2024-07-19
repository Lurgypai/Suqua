#pragma once

#include "DamageCalculator.h"
#include <cstdint>

class BasicDamageCalculator : public DamageCalculator {
public:

	BasicDamageCalculator(uint32_t damage_);
	// Inherited via DamageCalculator
	virtual uint32_t getDamage() const override;
	uint32_t damage;
};