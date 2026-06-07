#pragma once
#include <cstdint>

class DamageCalculator {
public:
    virtual ~DamageCalculator() {};
	virtual std::uint32_t getDamage() const = 0;
};
