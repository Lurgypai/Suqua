#pragma once
#include <cstdint>

class DamageCalculator {
public:
	virtual std::uint32_t getDamage() const = 0;
};